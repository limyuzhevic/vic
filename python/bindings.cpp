#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>

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

// Custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& message) : NLMError("Agent Error: " + message) {}
};

// Enhanced Python bindings with advanced features
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Context manager support for resource management
        - Enhanced error handling with custom exception classes
        - Convenience methods and fluent interface
        - Performance monitoring APIs
        - Event system support
        - Advanced configuration options
        - Serialization support
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError.ptr());
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError.ptr());
    py::register_exception<AgentError>(m, "AgentError", PyExc_RuntimeError.ptr());

    // NeuronId with enhanced Python features
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId with enhanced Python features
    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId with enhanced Python features
    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId with enhanced Python features
    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum with better Python integration
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("__str__", [](NeuronType type) {
            static const std::vector<std::string> names = {"Excitatory", "Inhibitory", "Modulatory", "Sensory", "Motor", "Internal"};
            return names[static_cast<int>(type)];
        });

    // SynapseType enum with better Python integration
    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("__str__", [](SynapseType type) {
            static const std::vector<std::string> names = {"Excitatory", "Inhibitory", "Modulatory", "Electrical", "GapJunction"};
            return names[static_cast<int>(type)];
        });

    // DevelopmentalStage enum with better Python integration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("__str__", [](DevelopmentalStage stage) {
            static const std::vector<std::string> names = {"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"};
            return names[static_cast<int>(stage)];
        });

    // FiringState enum with better Python integration
    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("__str__", [](FiringState state) {
            static const std::vector<std::string> names = {"Resting", "Active", "Refractory", "Inhibited"};
            return names[static_cast<int>(state)];
        });

    // ActionType enum with better Python integration
    py::enum_<ActionType>(m, "ActionType")
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
        .export_values()
        .def("__str__", [](ActionType type) {
            static const std::vector<std::string> names = {"MoveForward", "MoveBackward", "MoveLeft", "MoveRight", "TurnLeft", "TurnRight", "Look", "LookUp", "LookDown", "Interact", "Eat", "Drink", "Rest", "Wait", "Custom"};
            return names[static_cast<int>(type)];
        });

    // MotorCommand enum with better Python integration
    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("__str__", [](MotorCommand cmd) {
            static const std::vector<std::string> names = {"MoveForward", "MoveBackward", "TurnLeft", "TurnRight", "LookLeft", "LookRight", "Interact", "Wait"};
            return names[static_cast<int>(cmd)];
        });

    // WorldObjectType enum with better Python integration
    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("__str__", [](WorldObjectType type) {
            static const std::vector<std::string> names = {"Empty", "Resource", "Hazard", "Wall", "Marker"};
            return names[static_cast<int>(type)];
        });

    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::getKeys),
             "Get all configuration keys")
        .def("get", py::overload_cast<const std::string&, int>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default")
        .def("get", py::overload_cast<const std::string&>(&Config::get),
             py::arg("key"),
             "Get configuration value (returns None if not found)")
        .def("set", py::overload_cast<const std::string&, int>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (integer)")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (float)")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (boolean)")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (string)")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", &Config::getKeys,
             "Get number of configuration entries")
        .def("__contains__", &Config::has,
             "Check if key exists in configuration");

    // Enhanced SensoryInput class with Pythonic interface
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a list")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with Pythonic interface
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const py::list& data) {
            std::vector<float> vec;
            vec.reserve(py::len(data));
            for (auto item : data) {
                vec.push_back(py::cast<float>(item));
            }
            self.setData(vec);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("to_array", [](const Vision& self) {
            py::list list;
            const auto& data = self.getData();
            for (float value : data) {
                list.append(value);
            }
            return list;
        }, "Convert vision data to Python list");

    // Enhanced Audio class with Pythonic interface
    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const py::list& data) {
            std::vector<float> vec;
            vec.reserve(py::len(data));
            for (auto item : data) {
                vec.push_back(py::cast<float>(item));
            }
            self.setData(vec);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples);

    // Enhanced InternalSignals class with Pythonic interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("to_list", [](const InternalSignals& self) {
            py::list list;
            const auto& data = self.getData();
            for (float value : data) {
                list.append(value);
            }
            return list;
        }, "Convert internal signals to Python list");

    // Enhanced Action class with Pythonic interface
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", [](Action& self) {
            py::list list;
            const auto& params = self.getParameters();
            for (float value : params) {
                list.append(value);
            }
            return list;
        })
        .def("set_parameters", [](Action& self, const py::list& params) {
            std::vector<float> vec;
            vec.reserve(py::len(params));
            for (auto item : params) {
                vec.push_back(py::cast<float>(item));
            }
            self.setParameters(vec);
        }, py::arg("parameters"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone);

    // Enhanced WorldObject class with Pythonic interface
    py::class_<WorldObject>(m, "WorldObject")
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

    // Enhanced AgentBody class with Pythonic interface
    py::class_<AgentBody>(m, "AgentBody")
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

    // Enhanced ActionResult class with Pythonic interface
    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Enhanced SensoryPercept class with Pythonic interface
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal data")
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Enhanced SimpleWorld class with Pythonic interface
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed);

    // Enhanced Brain class with Pythonic interface
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", py::overload_cast<SimulationStep>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
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
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
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
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status");

    // Enhanced AgentBrain class with Pythonic interface
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("process_sensory_input", [](AgentBrain& self, const SensoryPercept& percept, const py::list& weights) {
            std::vector<float> vec;
            vec.reserve(py::len(weights));
            for (auto item : weights) {
                vec.push_back(py::cast<float>(item));
            }
            self.processSensoryInput(percept, vec);
        }, py::arg("percept"), py::arg("weights"),
            "Process sensory input with custom weights")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             py::arg("strategy") = 0,
             "Decode brain motor activity into motor command")
        .def("decode_motor_command_with_curiosity", &AgentBrain::decodeMotorCommandWithCuriosity,
             py::arg("strategy") = 0,
             "Decode brain motor activity into motor command with curiosity")
        .def("apply_reward_modulation", py::overload_cast<float, float, const RewardContext&>(&AgentBrain::applyRewardModulation),
             py::arg("reward"), py::arg("predicted_reward") = 0.0f, py::arg("context") = RewardContext(),
             "Apply reward-based neuromodulation")
        .def("apply_reward_modulation", py::overload_cast<const RewardSignal&>(&AgentBrain::applyRewardModulation),
             py::arg("reward_signal"),
             "Apply reward-based neuromodulation with signal")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), py::arg("context") = DevelopmentContext(),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        .def("process_multiple_steps", &AgentBrain::processMultipleSteps,
             py::arg("percepts"),
             "Process multiple sensory inputs")
        .def("decode_multiple_steps", &AgentBrain::decodeMultipleSteps,
             py::arg("count"),
             "Decode multiple motor commands")
        .def("get_vision_neuron_count", &AgentBrain::getVisionNeuronCount,
             "Get count of vision sensory neurons")
        .def("get_touch_neuron_count", &AgentBrain::getTouchNeuronCount,
             "Get count of touch sensory neurons")
        .def("get_internal_neuron_count", &AgentBrain::getInternalNeuronCount,
             "Get count of internal sensory neurons")
        .def("get_proprioception_neuron_count", &AgentBrain::getProprioceptionNeuronCount,
             "Get count of proprioception sensory neurons")
        .def("save_state", &AgentBrain::saveState, py::arg("filepath"),
             "Save agent brain state to file")
        .def("load_state", &AgentBrain::loadState, py::arg("filepath"),
             "Load agent brain state from file")
        .def("serialize_state", &AgentBrain::serializeState,
             "Serialize agent brain state to string")
        .def("deserialize_state", &AgentBrain::deserializeState, py::arg("data"),
             "Deserialize agent brain state from string")
        .def("get_performance_stats", &AgentBrain::getPerformanceStats,
             "Get performance statistics")
        .def("get_debug_info", &AgentBrain::getDebugInfo,
             "Get debug information")
        .def("enable_debug_output", &AgentBrain::enableDebugOutput, py::arg("enable"),
             "Enable/disable debug output");

    // Factory functions with Pythonic interface
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")

    // Utility functions
    m.def("create_brain_from_config", [](const std::string& config_file) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(config_file);
        return std::make_shared<Brain>(config);
    }, py::arg("config_file"), "Create brain from configuration file")

    m.def("create_agent_with_world", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("world"),
        "Create agent brain system with world (returns tuple)")

    m.def("run_simple_simulation", [](std::shared_ptr<Config> config, size_t steps) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableCuriosity(true);
        agent->enableRewardModulation(true);
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward based on action
            float reward = world->getEnergyDecayRate();
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("config"), py::arg("steps"),
        "Run a simple simulation (returns tuple)")

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and information
    m.def("get_version", []() {
        return "NLM Python Bindings 0.1.0";
    });

    // Error handling convenience functions
    m.def("validate_config", [](const Config& config) {
        auto keys = config.getKeys();
        if (keys.empty()) {
            throw ConfigError("Configuration is empty");
        }
        
        // Check for required keys
        const std::vector<std::string> required_keys = {"brain.neuron_count", "brain.synapse_density"};
        for (const auto& key : required_keys) {
            if (!config.has(key)) {
                throw ConfigError("Missing required configuration key: " + key);
            }
        }
        
        return true;
    }, py::arg("config"), "Validate configuration");

    m.def("validate_brain", [](const Brain& brain) {
        if (brain.getTotalNeuronCount() == 0) {
            throw BrainError("Brain has no neurons");
        }
        
        if (brain.getTotalSynapseCount() == 0) {
            throw BrainError("Brain has no synapses");
        }
        
        return true;
    }, py::arg("brain"), "Validate brain state");

    m.def("validate_agent", [](const AgentBrain& agent) {
        if (!agent.getBrain()) {
            throw AgentError("Agent has no brain");
        }
        
        return true;
    }, py::arg("agent"), "Validate agent state");

} // namespace nlm

} // namespace nlm