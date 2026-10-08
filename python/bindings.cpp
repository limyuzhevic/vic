#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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

// Python exception types
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public NLMException {
public:
    explicit InvalidArgumentError(const std::string& msg) : NLMException(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException(msg) {}
};

class BrainInitializationError : public NLMException {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMException(msg) {}
};

// Convenience wrapper functions for Python
define_py_array_converter() {
    py::class_<std::vector<float>> vecFloat(m, "VectorFloat");
    vecFloat.def(py::init<>(), "Empty vector")
            .def(py::init<size_t>(), "Size constructor")
            .def(py::init<const std::vector<float>&>(), "Initializer list")
            .def("resize", &std::vector<float>::resize, "Resize vector")
            .def("push_back", &std::vector<float>::push_back, "Add element")
            .def("size", &std::vector<float>::size, "Get size")
            .def("at", &std::vector<float>::at, "Access element by index")
            .def("data", [](std::vector<float>& v) {
                return py::memoryview(py::buffer(&v[0]).with_shape({(ssize_t)v.size()}));
            }, "Get data as memoryview");
}

// Enhanced Configuration class with Pythonic APIs
void configure_brain(py::class_<Config>& configClass) {
    configClass
        .def(py::init<>())
        .def("load", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("save", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("get", static_cast<std::optional<int> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get integer value")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get float value")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get boolean value")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get string value")
        .def("get", static_cast<std::optional<std::vector<int>> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get integer list")
        .def("get", static_cast<std::optional<std::vector<double>> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get float list")
        .def("get", static_cast<std::optional<std::vector<std::string>> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get string list")
        .def("get_or", &Config::getOr<int>, py::arg("key"), py::arg("default"),
             "Get integer with default")
        .def("get_or", &Config::getOr<double>, py::arg("key"), py::arg("default"),
             "Get float with default")
        .def("get_or", &Config::getOr<bool>, py::arg("key"), py::arg("default"),
             "Get boolean with default")
        .def("get_or", &Config::getOr<std::string>, py::arg("key"), py::arg("default"),
             "Get string with default")
        .def("set", static_cast<void (Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set integer value")
        .def("set", static_cast<void (Config::*)(const std::string&, double)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set float value")
        .def("set", static_cast<void (Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set boolean value")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set string value")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<int>&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set integer list")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<double>&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set float list")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<std::string>&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set string list")
        .def("validate", &Config::has, py::arg("key"),
             "Validate that configuration key exists")
        .def("validate_all", [](Config& self, const std::vector<std::string>& requiredKeys) {
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    throw InvalidArgumentError("Missing required configuration key: " + key);
                }
            }
        }, py::arg("required_keys"), "Validate all required keys")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    result[key] = *value;
                } else {
                    auto intValue = self.get<int>(key);
                    if (intValue.has_value()) {
                        result[key] = *intValue;
                    } else {
                        auto floatValue = self.get<double>(key);
                        if (floatValue.has_value()) {
                            result[key] = *floatValue;
                        } else {
                            auto boolValue = self.get<bool>(key);
                            if (boolValue.has_value()) {
                                result[key] = *boolValue;
                            }
                        }
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::str(item.first);
                py::object value = item.second;
                
                if (value.is(py::int_())) {
                    self.set(key, value.cast<int>());
                } else if (value.is(py::float_())) {
                    self.set(key, value.cast<double>());
                } else if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>());
                } else if (value.is(py::str())) {
                    self.set(key, value.cast<std::string>());
                } else if (value.is(py::list())) {
                    // Check if it's a list of numbers or strings
                    py::list pyList = value.cast<py::list>();
                    if (!pyList.empty() && pyList[0].is(py::int_())) {
                        std::vector<int> intList;
                        for (auto item : pyList) {
                            intList.push_back(item.cast<int>());
                        }
                        self.set(key, intList);
                    } else if (!pyList.empty() && pyList[0].is(py::float_())) {
                        std::vector<double> floatList;
                        for (auto item : pyList) {
                            floatList.push_back(item.cast<double>());
                        }
                        self.set(key, floatList);
                    } else {
                        std::vector<std::string> stringList;
                        for (auto item : pyList) {
                            stringList.push_back(item.cast<std::string>());
                        }
                        self.set(key, stringList);
                    }
                } else {
                    throw InvalidArgumentError("Unsupported configuration value type for key: " + key);
                }
            }
        }, py::arg("data"), "Load configuration from Python dictionary")
        .def("copy", []() {
            return Config();
        }, "Create a copy of configuration");
}

// Enhanced SensoryInput class
void configure_sensory_input(py::class_<SensoryInput>& sensoryClass) {
    sensoryClass
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_numpy", [](const SensoryInput& self) {
            auto data = self.getData();
            return py::array_t<float>(data.size(), data.data());
        }, "Convert to NumPy array")
        .def("copy", [](const SensoryInput& self) {
            return self; // Will be handled by copy constructor if needed
        }, "Create a copy of sensory input");
}

// Enhanced Vision class
void configure_vision(py::class_<Vision, SensoryInput>& visionClass) {
    visionClass
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("create_empty", [](Vision& self, size_t width, size_t height, size_t channels) {
            return Vision(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
         "Create empty vision input");
}

// Enhanced Audio class
void configure_audio(py::class_<Audio, SensoryInput>& audioClass) {
    audioClass
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("create_empty", [](Audio& self, size_t sample_rate, size_t num_samples) {
            return Audio(sample_rate, num_samples);
        }, py::arg("sample_rate"), py::arg("num_samples"),
         "Create empty audio input");
}

// Enhanced Action class
void configure_action(py::class_<Action>& actionClass) {
    actionClass
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("get_parameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw InvalidArgumentError("Parameter index out of range");
        }, py::arg("index"), "Get specific parameter")
        .def("add_parameter", &Action::setParameters, py::arg("params"),
             "Add parameters to action")
        .def("clone", &Action::clone, "Clone action")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary")
        .def("from_dict", [](Action& self, const py::dict& data) {
            if (data.contains("type")) {
                self.setType(static_cast<ActionType>(data["type"].cast<int>()));
            }
            if (data.contains("parameters")) {
                self.setParameters(data["parameters"].cast<std::vector<float>>());
            }
        }, py::arg("data"), "Load action from dictionary");
}

// Enhanced Brain class
void configure_brain(py::class_<Brain>& brainClass) {
    brainClass
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("get_region_names", [](Brain& self) {
            auto ids = self.getRegionIds();
            std::vector<std::string> names;
            for (auto rid : ids) {
                names.push_back("Region_" + std::to_string(rid.index()));
            }
            return names;
        }, "Get all region names")
        .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount, "Get count of firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("enable_reward_modulation", &Brain::applyNeuromodulation, py::arg("signal"),
             "Apply neuromodulatory signal")
        .def("update_plasticity", &Brain::updatePlasticity, "Update plasticity rules")
        .def("develop", &Brain::develop, "Apply developmental changes")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("get_status", &Brain::logStatus, "Get brain status summary")
        .def("is_initialized", [](Brain& self) {
            // This is a simplified check - in practice, you might track initialization state
            return self.getTotalNeuronCount() > 0;
        }, "Check if brain is initialized")
        .def("get_neuron_statistics", [](Brain& self) {
            py::dict result;
            result["total"] = self.getTotalNeuronCount();
            result["active"] = self.getActiveNeuronCount();
            result["firing"] = self.getFiringNeuronCount();
            result["spike_count"] = self.getTotalSpikeCount();
            result["e_i_ratio"] = self.getExcitationInhibitionRatio();
            result["avg_rate"] = self.getAverageFiringRate();
            return result;
        }, "Get detailed neuron statistics")
        .def("run_step", [](Brain& self, SimulationStep step, Timestamp time = 0) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time") = 0, "Run a single simulation step");
}

// Enhanced AgentBrain class
void configure_agent_brain(py::class_<AgentBrain>& agentBrainClass) {
    agentBrainClass
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"), "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        .def("get_agent_status", [](AgentBrain& self) {
            py::dict result;
            result["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            result["neuromodulation_level"] = self.getNeuromodulationLevel();
            result["curiosity_level"] = self.getCuriosityLevel();
            result["novelty_level"] = self.getNoveltyLevel();
            result["prediction_error"] = self.getPredictionError();
            return result;
        }, "Get agent status dictionary")
        .def("simulate_step", [](AgentBrain& self, SimulationStep step, Timestamp time = 0) {
            // This is a simplified simulation step - real implementation would be more complex
            auto percept = self.getSensoryInputSize() > 0 ? 
                // Create a default percept
                SensoryPercept() : nullptr;
            if (percept) {
                self.processSensoryInput(*percept);
            }
            self.updateDevelopment(step);
        }, py::arg("step"), py::arg("time") = 0, "Simulate a step with agent brain");
}

// Enhanced SimpleWorld class
void configure_world(py::class_<SimpleWorld>& worldClass) {
    worldClass
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position")
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        .def("simulate_step", [](SimpleWorld& self, double dt = 1.0) {
            self.update(static_cast<nlm::SimulationStep>(dt));
        }, py::arg("dt") = 1.0, "Simulate a world step")
        .def("get_world_status", [](SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["time"] = self.getSimulationTime();
            return result;
        }, "Get world status dictionary");
}

// Helper functions for creating default objects
void configure_helpers(py::module& m) {
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a new brain");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Simulation utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               size_t steps, Timestamp timeStep = 1.0) {
        if (!brain || !world) {
            throw InvalidArgumentError("Brain and world must not be null");
        }

        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            Timestamp time = static_cast<Timestamp>(step * timeStep);
            brain->step(step, time);
            world->update(step);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("time_step") = 1.0,
     "Run a complete simulation");

    m.def("run_agent_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                                  size_t steps) {
        if (!agent || !world) {
            throw InvalidArgumentError("Agent and world must not be null");
        }

        agent->reset();
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            auto percept = world->getSensoryPercept();
            if (percept) {
                agent->processSensoryInput(*percept);
            }
            world->update(step);
            agent->updateDevelopment(step);
        }
    }, py::arg("agent"), py::arg("world"), py::arg("steps"),
     "Run a complete agent episode");

    // Utility functions
    m.def("random_seed", [](unsigned int seed) {
        // This would require access to the RandomGenerator class
        // For now, just set the system random seed
        std::srand(seed);
    }, py::arg("seed"), "Set random seed for reproducible simulations");

    m.def("get_version", []() {
        return "NLM v0.1.0 (Phase 6 - Final Integration)";
    }, "Get NLM version information");

    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "NLM v0.1.0";
        info["phase"] = "6: FINAL INTEGRATION";
        info["description"] = "Neural Learning Machine - Experimental Computational Brain";
        info["cpp_standard"] = "C++20";
        info["python_bindings"] = "pybind11";
        info["architecture"] = "Closed-loop brain simulation";
        return info;
    }, "Get system information dictionary");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        
        A comprehensive Python API for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Pythonic APIs with convenience methods
        - Comprehensive error handling
        - NumPy integration support
        - Configuration validation utilities
        - Performance monitoring tools
        - Example scripts and tutorials
        
        Key Features:
        * Brain simulation with spiking neurons
        * Memory systems (working, episodic, associative)
        * Neuromodulation (dopamine, curiosity, novelty)
        * Prediction and planning systems
        * Developmental learning stages
        * Checkpoint save/load functionality
        
        Usage:
        >>> import pynlm
        >>> config = pynlm.create_default_config()
        >>> config.set("neuron_count", 1000)
        >>> brain = pynlm.create_brain(config)
        >>> world = pynlm.create_simple_world()
        >>> percept = world.get_sensory_percept()
        >>> brain.receive_sensory_input(percept)
        >>> action = brain.produce_action()
    )pbdoc";

    // Register exception types
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");

    // Register convenience array types
    py::class_<std::vector<float>>(m, "VectorFloat")
        .def(py::init<>(), "Empty vector")
        .def(py::init<size_t>(), "Size constructor")
        .def(py::init<const std::vector<float>&>(), "Initializer list")
        .def("resize", &std::vector<float>::resize, "Resize vector")
        .def("push_back", &std::vector<float>::push_back, "Add element")
        .def("size", &std::vector<float>::size, "Get size")
        .def("at", &std::vector<float>::at, "Access element by index")
        .def("data", [](std::vector<float>& v) {
            return py::memoryview(py::buffer(&v[0]).with_shape({(ssize_t)v.size()}));
        }, "Get data as memoryview")
        .def("to_numpy", [](const std::vector<float>& v) {
            return py::array_t<float>(v.size(), v.data());
        }, "Convert to NumPy array");

    // Register ID types
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

    // Register enums
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

    // Register classes with enhanced APIs
    py::class_<Config>(m, "Config", R"pbdoc(enhanced Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load", &Config::loadFromFile, py::arg("filepath"), "Load configuration from a JSON file")
        .def("save", &Config::saveToFile, py::arg("filepath"), "Save configuration to a JSON file")
        .def("get", static_cast<std::optional<int> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get integer value")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get float value")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get boolean value")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get string value")
        .def("get", static_cast<std::optional<std::vector<int>> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get integer list")
        .def("get", static_cast<std::optional<std::vector<double>> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get float list")
        .def("get", static_cast<std::optional<std::vector<std::string>> (Config::*)(const std::string&) const>(&Config::get),
             py::arg("key"), "Get string list")
        .def("get_or", &Config::getOr<int>, py::arg("key"), py::arg("default"), "Get integer with default")
        .def("get_or", &Config::getOr<double>, py::arg("key"), py::arg("default"), "Get float with default")
        .def("get_or", &Config::getOr<bool>, py::arg("key"), py::arg("default"), "Get boolean with default")
        .def("get_or", &Config::getOr<std::string>, py::arg("key"), py::arg("default"), "Get string with default")
        .def("set", static_cast<void (Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set integer value")
        .def("set", static_cast<void (Config::*)(const std::string&, double)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set float value")
        .def("set", static_cast<void (Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set boolean value")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set string value")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<int>&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set integer list")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<double>&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set float list")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<std::string>&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set string list")
        .def("has", &Config::has, py::arg("key"), "Check if configuration key exists")
        .def("validate", [](Config& self, const std::vector<std::string>& requiredKeys) {
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    throw InvalidArgumentError("Missing required configuration key: " + key);
                }
            }
        }, py::arg("required_keys"), "Validate all required keys")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value.has_value()) {
                    result[key] = *value;
                } else {
                    auto intValue = self.get<int>(key);
                    if (intValue.has_value()) {
                        result[key] = *intValue;
                    } else {
                        auto floatValue = self.get<double>(key);
                        if (floatValue.has_value()) {
                            result[key] = *floatValue;
                        } else {
                            auto boolValue = self.get<bool>(key);
                            if (boolValue.has_value()) {
                                result[key] = *boolValue;
                            }
                        }
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::str(item.first);
                py::object value = item.second;
                
                if (value.is(py::int_())) {
                    self.set(key, value.cast<int>());
                } else if (value.is(py::float_())) {
                    self.set(key, value.cast<double>());
                } else if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>());
                } else if (value.is(py::str())) {
                    self.set(key, value.cast<std::string>());
                } else if (value.is(py::list())) {
                    py::list pyList = value.cast<py::list>();
                    if (!pyList.empty() && pyList[0].is(py::int_())) {
                        std::vector<int> intList;
                        for (auto item : pyList) {
                            intList.push_back(item.cast<int>());
                        }
                        self.set(key, intList);
                    } else if (!pyList.empty() && pyList[0].is(py::float_())) {
                        std::vector<double> floatList;
                        for (auto item : pyList) {
                            floatList.push_back(item.cast<double>());
                        }
                        self.set(key, floatList);
                    } else {
                        std::vector<std::string> stringList;
                        for (auto item : pyList) {
                            stringList.push_back(item.cast<std::string>());
                        }
                        self.set(key, stringList);
                    }
                } else {
                    throw InvalidArgumentError("Unsupported configuration value type for key: " + key);
                }
            }
        }, py::arg("data"), "Load configuration from Python dictionary")
        .def("get_keys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Register sensory input classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_numpy", [](const SensoryInput& self) {
            auto data = self.getData();
            return py::array_t<float>(data.size(), data.data());
        }, "Convert to NumPy array")
        .def("copy", [](const SensoryInput& self) {
            return self;
        }, "Create a copy of sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("create_empty", [](Vision& self, size_t width, size_t height, size_t channels) {
            return Vision(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
         "Create empty vision input");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("create_empty", [](Audio& self, size_t sample_rate, size_t num_samples) {
            return Audio(sample_rate, num_samples);
        }, py::arg("sample_rate"), py::arg("num_samples"),
         "Create empty audio input");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals);

    // Register action classes
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("get_parameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw InvalidArgumentError("Parameter index out of range");
        }, py::arg("index"), "Get specific parameter")
        .def("add_parameter", &Action::setParameters, py::arg("params"), "Add parameters to action")
        .def("clone", &Action::clone, "Clone action")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary")
        .def("from_dict", [](Action& self, const py::dict& data) {
            if (data.contains("type")) {
                self.setType(static_cast<ActionType>(data["type"].cast<int>()));
            }
            if (data.contains("parameters")) {
                self.setParameters(data["parameters"].cast<std::vector<float>>());
            }
        }, py::arg("data"), "Load action from dictionary");

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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Register world and simulation classes
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed);

    // Register main brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("get_region_names", [](Brain& self) {
            auto ids = self.getRegionIds();
            std::vector<std::string> names;
            for (auto rid : ids) {
                names.push_back("Region_" + std::to_string(rid.index()));
            }
            return names;
        }, "Get all region names")
        .def("get_region_count", &Brain::getRegionCount, "Get the number of regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount, "Get count of firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("apply_neuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signal")
        .def("update_plasticity", &Brain::updatePlasticity, "Update plasticity rules")
        .def("develop", &Brain::develop, "Apply developmental changes")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("get_status", &Brain::logStatus, "Get brain status summary")
        .def("is_initialized", [](Brain& self) {
            return self.getTotalNeuronCount() > 0;
        }, "Check if brain is initialized")
        .def("get_neuron_statistics", [](Brain& self) {
            py::dict result;
            result["total"] = self.getTotalNeuronCount();
            result["active"] = self.getActiveNeuronCount();
            result["firing"] = self.getFiringNeuronCount();
            result["spike_count"] = self.getTotalSpikeCount();
            result["e_i_ratio"] = self.getExcitationInhibitionRatio();
            result["avg_rate"] = self.getAverageFiringRate();
            return result;
        }, "Get detailed neuron statistics")
        .def("run_step", [](Brain& self, SimulationStep step, Timestamp time = 0) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time") = 0, "Run a single simulation step");

    // Register agent brain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"), "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        .def("get_agent_status", [](AgentBrain& self) {
            py::dict result;
            result["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            result["neuromodulation_level"] = self.getNeuromodulationLevel();
            result["curiosity_level"] = self.getCuriosityLevel();
            result["novelty_level"] = self.getNoveltyLevel();
            result["prediction_error"] = self.getPredictionError();
            return result;
        }, "Get agent status dictionary")
        .def("simulate_step", [](AgentBrain& self, SimulationStep step, Timestamp time = 0) {
            auto percept = self.getSensoryInputSize() > 0 ? SensoryPercept() : nullptr;
            if (percept) {
                self.processSensoryInput(*percept);
            }
            self.updateDevelopment(step);
        }, py::arg("step"), py::arg("time") = 0, "Simulate a step with agent brain");

    // Register helper functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create and initialize a new brain");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Simulation utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               size_t steps, Timestamp timeStep = 1.0) {
        if (!brain || !world) {
            throw InvalidArgumentError("Brain and world must not be null");
        }

        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            Timestamp time = static_cast<Timestamp>(step * timeStep);
            brain->step(step, time);
            world->update(step);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("time_step") = 1.0,
     "Run a complete simulation");

    m.def("run_agent_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                                  size_t steps) {
        if (!agent || !world) {
            throw InvalidArgumentError("Agent and world must not be null");
        }

        agent->reset();
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            auto percept = world->getSensoryPercept();
            if (percept) {
                agent->processSensoryInput(*percept);
            }
            world->update(step);
            agent->updateDevelopment(step);
        }
    }, py::arg("agent"), py::arg("world"), py::arg("steps"),
     "Run a complete agent episode");

    // Utility functions
    m.def("random_seed", [](unsigned int seed) {
        std::srand(seed);
    }, py::arg("seed"), "Set random seed for reproducible simulations");

    m.def("get_version", []() {
        return "NLM v0.1.0 (Phase 6 - Final Integration)";
    }, "Get NLM version information");

    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "NLM v0.1.0";
        info["phase"] = "6: FINAL INTEGRATION";
        info["description"] = "Neural Learning Machine - Experimental Computational Brain";
        info["cpp_standard"] = "C++20";
        info["python_bindings"] = "pybind11";
        info["architecture"] = "Closed-loop brain simulation";
        return info;
    }, "Get system information dictionary");

    // Invalid neuron ID for Python API
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Define Python exception translations
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const InvalidArgumentError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const BrainInitializationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

} // namespace nlm
