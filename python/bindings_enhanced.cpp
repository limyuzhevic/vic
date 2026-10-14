#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
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
#include "../src/agent/AgentBrain.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/development/DevelopmentSystem.hpp"

// Forward declarations for convenience
template<typename T>
using SharedPtr = std::shared_ptr<T>;

namespace py = pybind11;
namespace nlm {

// Type aliases for cleaner bindings
typedef std::shared_ptr<Config> ConfigPtr;
typedef std::shared_ptr<Brain> BrainPtr;
typedef std::shared_ptr<AgentBrain> AgentBrainPtr;
typedef std::shared_ptr<SimpleWorld> WorldPtr;
typedef std::shared_ptr<SensoryInput> SensoryInputPtr;
typedef std::shared_ptr<Vision> VisionPtr;
typedef std::shared_ptr<Audio> AudioPtr;
typedef std::shared_ptr<InternalSignals> InternalSignalsPtr;
typedef std::shared_ptr<Action> ActionPtr;
typedef std::shared_ptr<WorldObject> WorldObjectPtr;
typedef std::shared_ptr<AgentBody> AgentBodyPtr;
typedef std::shared_ptr<ActionResult> ActionResultPtr;
typedef std::shared_ptr<SensoryPercept> SensoryPerceptPtr;
typedef std::shared_ptr<Neuromodulator> NeuromodulatorPtr;
typedef std::shared_ptr<PredictionSystem> PredictionSystemPtr;
typedef std::shared_ptr<Memory> MemoryPtr;
typedef std::shared_ptr<NeuralPlanner> NeuralPlannerPtr;
typedef std::shared_ptr<DevelopmentSystem> DevelopmentSystemPtr;
typedef std::shared_ptr<VisualizationInterface> VisualizationInterfacePtr;

// Helper functions for better Python integration

// Convert NeuronId to Python string representation
std::string neuron_id_to_string(const NeuronId& id) {
    return "<NeuronId: " + std::to_string(id.value) + ">";
}

// Convert SynapseId to Python string representation  
std::string synapse_id_to_string(const SynapseId& id) {
    return "<SynapseId: " + std::to_string(id.value) + ">";
}

// Convert RegionId to Python string representation
std::string region_id_to_string(const RegionId& id) {
    return "<RegionId: " + std::to_string(id.value) + ">";
}

// Convert PopulationId to Python string representation
std::string population_id_to_string(const PopulationId& id) {
    return "<PopulationId: " + std::to_string(id.value) + ">";
}

// Create comprehensive configuration with validation
ConfigPtr createValidatedConfig(const std::map<std::string, py::object>& params) {
    auto config = std::make_shared<Config>();
    
    for (const auto& [key, value] : params) {
        // Try to convert based on Python type
        if (py::isinstance<py::int_>(value)) {
            config->set(key, value.cast<int64_t>(), ConfigSource::Runtime);
        } else if (py::isinstance<py::float_>(value)) {
            config->set(key, value.cast<double>(), ConfigSource::Runtime);
        } else if (py::isinstance<py::str>(value)) {
            config->set(key, value.cast<std::string>(), ConfigSource::Runtime);
        } else if (py::isinstance<py::bool_>(value)) {
            config->set(key, value.cast<bool>(), ConfigSource::Runtime);
        }
    }
    
    return config;
}

// Helper to convert simulation step to Python-compatible format
SimulationStep simulation_step_from_py(py::object step) {
    if (py::isinstance<py::int_>(step)) {
        return static_cast<SimulationStep>(step.cast<int64_t>());
    } else if (py::isinstance<py::float_>(step)) {
        return static_cast<SimulationStep>(step.cast<int64_t>());
    }
    throw std::runtime_error("Invalid simulation step type");
}

// Helper to convert timestamp to Python-compatible format
Timestamp timestamp_from_py(py::object time) {
    if (py::isinstance<py::int_>(time)) {
        return static_cast<Timestamp>(time.cast<int64_t>());
    } else if (py::isinstance<py::float_>(time)) {
        return static_cast<Timestamp>(time.cast<int64_t>());
    }
    throw std::runtime_error("Invalid timestamp type");
}

// Export convenience functions for common initialization patterns
void exportConvenienceFunctions(py::module& m) {
    m.def("createValidatedConfig", &createValidatedConfig,
          py::arg("params"),
          "Create a configuration with validated parameters from Python dict");
    
    m.def("simulation_step", &simulation_step_from_py,
          py::arg("step"),
          "Convert Python object to SimulationStep type");
    
    m.def("timestamp", &timestamp_from_py,
          py::arg("time"),
          "Convert Python object to Timestamp type");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive access to neural simulation capabilities
        including brain modeling, learning systems, memory, neuromodulation, and
        agent-based interaction with environments.
        
        Key features:
        - Full neural network simulation with LIF dynamics
        - Multiple memory systems (working, episodic, associative)
        - Neuromodulation (reward, curiosity, novelty)
        - Prediction and planning systems
        - Developmental stages and plasticity
        - Agent-environment interaction
        - Visualization and monitoring tools
        - Checkpoint/save-load functionality
        
        Usage examples are available in the examples/ directory.
    )pbdoc";

    // Install Python-style exception handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Forward declarations for complex classes
    py::class_<NeuronId> NeuronIdDecl(m, "NeuronId", R"pbdoc(Neuron identifier)pbdoc");
    py::class_<SynapseId> SynapseIdDecl(m, "SynapseId", R"pbdoc(Synapse identifier)pbdoc");
    py::class_<RegionId> RegionIdDecl(m, "RegionId", R"pbdoc(Brain region identifier)pbdoc");
    py::class_<PopulationId> PopulationIdDecl(m, "PopulationId", R"pbdoc(Neuron population identifier)pbdoc");
    
    // NeuronId class with enhanced Python interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", &neuron_id_to_string)
        .def("__str__", &neuron_id_to_string);
    
    // SynapseId class with enhanced Python interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", &synapse_id_to_string)
        .def("__str__", &synapse_id_to_string);
    
    // RegionId class with enhanced Python interface
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Brain region identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", &region_id_to_string)
        .def("__str__", &region_id_to_string);
    
    // PopulationId class with enhanced Python interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Neuron population identifier)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", &population_id_to_string)
        .def("__str__", &population_id_to_string);
    
    // NeuronType enum with better Python integration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .doc() = "Enumeration of neuron types in the neural network."
        ;
    
    // SynapseType enum with better Python integration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .doc() = "Enumeration of synapse types in the neural network."
        ;
    
    // DevelopmentalStage enum with better Python integration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .doc() = "Enumeration of developmental stages for neural development."
        ;
    
    // FiringState enum with better Python integration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .doc() = "Enumeration of neuron firing states."
        ;
    
    // ActionType enum with better Python integration
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
        .export_values()
        .doc() = "Enumeration of possible motor actions."
        ;
    
    // MotorCommand enum with better Python integration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .doc() = "Enumeration of low-level motor commands."
        ;
    
    // WorldObjectType enum with better Python integration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .doc() = "Enumeration of world object types."
        ;
    
    // Config class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration for NLM neural simulation)pbdoc")
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
        .def("get", &Config::get<int64_t>, py::arg("key"),
             "Get integer configuration value")
        .def("get", &Config::get<double>, py::arg("key"),
             "Get double configuration value")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get string configuration value")
        .def("get", &Config::get<bool>, py::arg("key"),
             "Get boolean configuration value")
        .def("set", py::overload_cast<const std::string&, int64_t, ConfigSource>(
            &Config::set), py::arg("key"), py::arg("value"),
            "Set integer configuration value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(
            &Config::set), py::arg("key"), py::arg("value"),
            "Set double configuration value")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(
            &Config::set), py::arg("key"), py::arg("value"),
            "Set string configuration value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(
            &Config::set), py::arg("key"), py::arg("value"),
            "Set boolean configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getOr", py::overload_cast<const std::string&, int64_t>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with fallback")
        .def("getOr", py::overload_cast<const std::string&, double>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with fallback")
        .def("getOr", py::overload_cast<const std::string&, const std::string&, &Config::getOr>(
            &Config::getOr), py::arg("key"), py::arg("default_value"),
             "Get configuration value with fallback")
        .def("getOr", py::overload_cast<const std::string&, bool>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with fallback")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("to_dict", [](const Config& cfg) {
            auto keys = cfg.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                if (cfg.has(key)) {
                    // Try to determine type and get appropriate value
                    if (cfg.get<int64_t>(key).has_value()) {
                        result[key] = cfg.get<int64_t>(key).value();
                    } else if (cfg.get<double>(key).has_value()) {
                        result[key] = cfg.get<double>(key).value();
                    } else if (cfg.get<std::string>(key).has_value()) {
                        result[key] = cfg.get<std::string>(key).value();
                    } else if (cfg.get<bool>(key).has_value()) {
                        result[key] = cfg.get<bool>(key).value();
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("updateFromDict", [](Config& self, const py::dict& params) {
            py::object keys = params.keys();nlm
            for (auto key : keys) {
                if (py::isinstance<py::str>(key)) {
                    py::object value = params[key];
                    if (py::isinstance<py::int_>(value)) {
                        self.set(key.cast<std::string>(), value.cast<int64_t>(), ConfigSource::Runtime);
                    } else if (py::isinstance<py::float_>(value)) {
                        self.set(key.cast<std::string>(), value.cast<double>(), ConfigSource::Runtime);
                    } else if (py::isinstance<py::str>(value)) {
                        self.set(key.cast<std::string>(), value.cast<std::string>(), ConfigSource::Runtime);
                    } else if (py::isinstance<py::bool_>(value)) {
                        self.set(key.cast<std::string>(), value.cast<bool>(), ConfigSource::Runtime);
                    }
                }
            }
        }, py::arg("params"), "Update configuration from Python dictionary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });
    
    // SensoryInput class with enhanced Python interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
    
    // Vision class with enhanced Python interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromNumpy", [](Vision& self, py::array_t<float> data) {
            auto buf = data.request();
            std::vector<float> vec(buf.size);
            std::copy(static_cast<float*>(buf.ptr),
                     static_cast<float*>(buf.ptr) + buf.size,
                     vec.begin());
            self.setData(vec);
        }, py::arg("data"), "Set data from NumPy array")
        .def("getDataAsNumpy", [](const Vision& self) {
            auto data = self.getData();
            return py::array_t<float>(data.size(), data.data());
        }, "Get data as NumPy array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);
    
    // Audio class with enhanced Python interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromNumpy", [](Audio& self, py::array_t<float> data) {
            auto buf = data.request();
            std::vector<float> vec(buf.size);
            std::copy(static_cast<float*>(buf.ptr),
                     static_cast<float*>(buf.ptr) + buf.size,
                     vec.begin());
            self.setData(vec);
        }, py::arg("data"), "Set data from NumPy array")
        .def("getDataAsNumpy", [](const Audio& self) {
            auto data = self.getData();
            return py::array_t<float>(data.size(), data.data());
        }, "Get data as NumPy array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
    
    // InternalSignals class with enhanced Python interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalsAsNumpy", [](const InternalSignals& self) {
            auto signals = self.getData();
            return py::array_t<float>(signals.size(), signals.data());
        }, "Get signals as NumPy array")
        .def("setSignalsFromNumpy", [](InternalSignals& self, py::array_t<float> signals) {
            auto buf = signals.request();
            std::vector<float> vec(buf.size);
            std::copy(static_cast<float*>(buf.ptr),
                     static_cast<float*>(buf.ptr) + buf.size,
                     vec.begin());
            self.setData(vec);
        }, py::arg("signals"), "Set signals from NumPy array");
    
    // Action class with enhanced Python interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            result["name"] = action.getName();
            return result;
        }, "Convert action to Python dictionary")
        .def("from_dict", [](Action& action, const py::dict& params) {
            if (params.contains("type")) {
                action.setType(py::cast<ActionType>(params["type"]));
            }
            if (params.contains("parameters")) {
                action.setParameters(py::cast<std::vector<float>>(params["parameters"]));
            }
        }, py::arg("params"), "Create action from Python dictionary");
    
    // WorldObject class with enhanced Python interface
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
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isType", [](const WorldObject& obj, WorldObjectType type) {
            return obj.type == type;
        }, py::arg("type"), "Check if object is of specific type");
    
    // AgentBody class with enhanced Python interface
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
        .def("distanceTo", [](const AgentBody& body, float x, float y) {
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("reset", &AgentBody::reset);
    
    // ActionResult class with enhanced Python interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if action was successful and rewarding")
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult: reward=" << result.reward
               << ", success=" << (result.success ? "true" : "false")
               << ", message=\"" << result.message << "\">";
            return ss.str();
        });
    
    // SensoryPercept class with enhanced Python interface
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            auto vision = percept.getVision();
            if (vision) result["vision"] = vision;
            auto touch = percept.getTouch();
            if (touch) result["touch"] = touch;
            auto internal = percept.getInternal();
            if (internal) result["internal"] = internal;
            auto proprioception = percept.getProprioception();
            if (proprioception) result["proprioception"] = proprioception;
            auto audio = percept.getAudio();
            if (audio) result["audio"] = audio;
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert percept to Python dictionary")
        .def("from_dict", [](SensoryPercept& percept, const py::dict& params) {
            if (params.contains("vision")) {
                percept.setVision(py::cast<std::shared_ptr<Vision>>(params["vision"]));
            }
            if (params.contains("touch")) {
                percept.setTouch(py::cast<std::shared_ptr<Touch>>(params["touch"]));
            }
            if (params.contains("internal")) {
                percept.setInternal(py::cast<std::shared_ptr<InternalSignals>>(params["internal"]));
            }
            if (params.contains("proprioception")) {
                percept.setProprioception(py::cast<std::shared_ptr<Proprioception>>(params["proprioception"]));
            }
            if (params.contains("audio")) {
                percept.setAudio(py::cast<std::shared_ptr<Audio>>(params["audio"]));
            }
            if (params.contains("timestamp")) {
                percept.setTimestamp(py::cast<Timestamp>(params["timestamp"]));
            }
        }, py::arg("params"), "Create percept from Python dictionary");
    
    // SimpleWorld class with enhanced Python interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory parameters")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state from world")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy for agents")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);
    
    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain)pbdoc")
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
             "Log brain status")
        // Convenience methods
        .def("simulate", [](Brain& self, int steps, bool verbose = false) {
            for (SimulationStep step = 0; step < steps; ++step) {
                self.step(step);
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << ": "
                             << self.getActiveNeuronCount() << " active neurons" << std::endl;
                }
            }
        }, py::arg("steps"), py::arg("verbose") = false,
           "Run simulation for specified number of steps")
        .def("getStatistics", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            return stats;
        }, "Get brain statistics as Python dictionary")
        .def("createCheckpoint", [](Brain& self, const std::string& description = "") {
            auto timestamp = std::chrono::system_clock::now();
            auto checkpoint_file = "brain_checkpoint_" + 
                                 std::to_string(std::chrono::system_clock::to_time_t(timestamp)) +
                                 ".json";
            self.save(checkpoint_file);
            return checkpoint_file;
        }, py::arg("description") = "",
           "Create checkpoint and return filename")
        .def("compareWith", [](const Brain& self1, const Brain& self2) {
            py::dict comparison;
            comparison["neuron_count_match"] = self1.getTotalNeuronCount() == self2.getTotalNeuronCount();
            comparison["synapse_count_match"] = self1.getTotalSynapseCount() == self2.getTotalSynapseCount();
            comparison["firing_rate_diff"] = std::abs(self1.getAverageFiringRate() - self2.getAverageFiringRate());
            return comparison;
        }, py::arg("other_brain"),
           "Compare two brains and return differences");
    
    // AgentBrain class with enhanced Python interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface)pbdoc")
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
        // Convenience methods
        .def("act", [](AgentBrain& self, const SensoryPercept& percept, ActionType action_type = ActionType::Wait) {
            ActionResult result;
            
            // Process sensory input
            self.processSensoryInput(percept);
            
            // Apply reward if provided
            if (action_type != ActionType::Wait) {
                float reward = 0.0f;
                if (action_type == ActionType::Eat || action_type == ActionType::Drink) {
                    reward = 10.0f;
                } else if (action_type == ActionType::Rest) {
                    reward = 5.0f;
                }
                self.applyRewardModulation(reward, 0.0f);
            }
            
            // Produce action
            auto action = self.getBrain()->produceAction();
            if (action) {
                // Apply development update
                self.updateDevelopment(1);
            }
            
            return action;
        }, py::arg("percept"), py::arg("action_type") = ActionType::Wait,
           "Convenience method to process input and produce action")
        .def("trainEpisode", [](AgentBrain& self, const WorldPtr& world, int max_steps = 100) {
            self.reset();
            world->reset();
            
            for (int step = 0; step < max_steps; ++step) {
                // Get sensory percept
                auto percept = world->getSensoryPercept();
                
                // Process and act
                auto action = self.act(*percept);
                
                if (action) {
                    // Apply action in world
                    world->applyMotorCommand(action->getType(), world->getSimulationTime());
                }
                
                // Update world
                world->update(1);
                
                // Check for termination
                auto agent_body = world->getAgentBody();
                if (agent_body->health <= 0.0f || agent_body->energy <= 0.0f) {
                    break;
                }
            }
        }, py::arg("world"), py::arg("max_steps") = 100,
           "Run a complete training episode");
    
    // Export convenience functions
    exportConvenienceFunctions(m);
    
    // Export factory functions for common patterns
    m.def("createDefaultConfig", []() -> ConfigPtr {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](ConfigPtr config) -> BrainPtr {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> WorldPtr {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](BrainPtr brain) -> AgentBrainPtr {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Export sentinel values for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Add module-level convenience functions
    m.def("createAdvancedConfig", [](const py::dict& params) {
        return createValidatedConfig(params);
    }, py::arg("params"), "Create configuration with Python dictionary");
    
    m.def("runQuickTest", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100, ConfigSource::Runtime);
        config->set("region_count", 2, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (SimulationStep step = 0; step < 10; ++step) {
            brain->step(step);
        }
        
        return brain;
    }, "Run a quick test and return brain for verification");
    
    m.def("getVersion", []() {
        return "NLM Python Bindings v1.0.0 (Enhanced)";
    }, "Get the version of the Python bindings");
    
    // Add utility functions
    m.def("format_neuron_id", &neuron_id_to_string,
          py::arg("id"), "Format neuron ID for display");
    
    m.def("format_synapse_id", &synapse_id_to_string,
          py::arg("id"), "Format synapse ID for display");
    
    m.def("format_region_id", &region_id_to_string,
          py::arg("id"), "Format region ID for display");
    
    m.def("format_population_id", &population_id_to_string,
          py::arg("id"), "Format population ID for display");
}

} // namespace nlm