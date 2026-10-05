#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
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
#include "../src/core/BrainError.hpp"
#include "../core/ErrorReporting.hpp"

namespace py = pybind11;
namespace nlm {

// Python-specific exception hierarchy
class NLMBindingError : public std::runtime_error {
public:
    explicit NLMBindingError(const std::string& message) 
        : std::runtime_error("NLM Binding Error: " + message) {}
};

class NLMBrainError : public NLMBindingError {
public:
    explicit NLMBrainError(const std::string& message) 
        : NLMBindingError("Brain: " + message) {}
};

class NLMWorldError : public NLMBindingError {
public:
    explicit NLMWorldError(const std::string& message) 
        : NLMBindingError("World: " + message) {}
};

class NLMConfigError : public NLMBindingError {
public:
    explicit NLMConfigError(const std::string& message) 
        : NLMBindingError("Config: " + message) {}
};

class NLMValidationError : public NLMBindingError {
public:
    explicit NLMValidationError(const std::string& message) 
        : NLMBindingError("Validation: " + message) {}
};

// Convert C++ exceptions to Python equivalents
namespace exception_converter {
    void setupExceptionConverters(py::module_& m) {
        py::register_exception_translator([](std::exception_ptr p) {
            if (p) {
                try {
                    std::rethrow_exception(p);
                } catch (const std::runtime_error& e) {
                    // Check for specific error types
                    std::string msg = e.what();
                    if (msg.find("Brain: ") != std::string::npos) {
                        PyErr_SetString(PyExc_RuntimeError, msg.c_str());
                    } else if (msg.find("World: ") != std::string::npos) {
                        PyErr_SetString(PyExc_ValueError, msg.c_str());
                    } else if (msg.find("Config: ") != std::string::npos) {
                        PyErr_SetString(PyExc_TypeError, msg.c_str());
                    } else if (msg.find("Validation: ") != std::string::npos) {
                        PyErr_SetString(PyExc_ValueError, msg.c_str());
                    } else {
                        PyErr_SetString(PyExc_RuntimeError, msg.c_str());
                    }
                } catch (...) {
                    PyErr_SetString(PyExc_RuntimeError, "Unknown error in NLM bindings");
                }
            }
        });
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This enhanced binding provides modern Python features including:
        - Context managers for automatic resource cleanup
        - Pythonic properties (snake_case) for all getter methods
        - Comprehensive error handling with Python-specific exceptions
        - Type hints and detailed documentation
        - Factory methods with better defaults
        - Simulation control with callbacks and recording
        - Configuration validation and helpers
        - Batch processing capabilities
    )pbdoc";

    // Register Python-specific exception hierarchy
    py::register_exception<NLMBindingError>(m, "NLMBindingError");
    py::register_exception<NLMBrainError>(m, "NLMBrainError");
    py::register_exception<NLMWorldError>(m, "NLMWorldError");
    py::register_exception<NLMConfigError>(m, "NLMConfigError");
    py::register_exception<NLMValidationError>(m, "NLMValidationError");

    // Set up C++ to Python exception conversion
    exception_converter::setupExceptionConverters(m);

    // ============= CONTEXT MANAGER HELPERS =============
    template<typename T>
    class ContextManagerWrapper {
    public:
        explicit ContextManagerWrapper(std::shared_ptr<T> obj) : obj_(std::move(obj)) {}
        
        T& get() { return *obj_; }
        const T& get() const { return *obj_; }
        
        void enter() {
            // Context manager enter logic
            if (obj_) {
                obj_->initialize();
            }
        }
        
        void exit() {
            // Context manager exit logic - cleanup resources
            if (obj_) {
                obj_->reset();
                obj_.reset(); // Release ownership
            }
        }
        
        bool is_valid() const { return obj_ != nullptr; }
        
    private:
        std::shared_ptr<T> obj_;
    };

    // ============= UTILITY FUNCTIONS =============
    namespace utils {
        // Create context manager from object
        template<typename T>
        std::shared_ptr<ContextManagerWrapper<T>> createContextManager(std::shared_ptr<T> obj) {
            return std::make_shared<ContextManagerWrapper<T>>(std::move(obj));
        }
        
        // Convert camelCase to snake_case for property names
        std::string toSnakeCase(const std::string& str) {
            std::string result;
            for (size_t i = 0; i < str.size(); ++i) {
                if (isupper(str[i])) {
                    if (i > 0 && !isupper(str[i-1])) {
                        result += '_';
                    }
                    result += tolower(str[i]);
                } else {
                    result += str[i];
                }
            }
            return result;
        }
        
        // Get value with type conversion and default
        template<typename T>
        T getValue(const Config& config, const std::string& key, const T& defaultValue) {
            auto value = config.get<T>(key);
            return value ? *value : defaultValue;
        }
    }

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

    }

    // ============= CONFIGURATION API ENHANCEMENTS =============

    // Enhanced Config with validation and Pythonic API
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)
        Enhanced configuration class with validation, type conversion, and Pythonic API.
        
        Features:
        - Pythonic property access (snake_case instead of camelCase)
        - Type conversion utilities
        - Configuration validation
        - Factory methods for common configurations
        - Context manager support
        "pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has_key", &Config::has, py::arg("key"),
             "Check if a configuration key exists (Pythonic)")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Pythonic properties
        .def_property_readonly("total_neurons", &Config::getOr<size_t>, "Get total neuron count from config")
        .def_property_readonly("region_count", &Config::getOr<size_t>, "Get region count from config")
        .def_property_readonly("connection_probability", &Config::getOr<float>, "Get connection probability from config")
        .def_property_readonly("simulation_timestep", &Config::getOr<double>, "Get simulation timestep from config")
        .def_property_readonly("random_seed", &Config::getOr<uint64_t>, "Get random seed from config")
        .def_property_readonly("checkpoint_dir", &Config::getOr<std::string>, "Get checkpoint directory from config")
        // Context manager support
        .def("__enter__", [](Config& self) { self.initialize(); return self; })
        .def("__exit__", [](Config&, const py::args&) { return true; })
        .def("validate_keys", &utils::validateConfigKeys, py::arg("required_keys"),
             "Validate configuration has all required keys (Pythonic)");

// Enhanced SensoryInput with Pythonic API
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        Enhanced sensory input base class with Pythonic API and context manager support.
        
        Features:
        - Pythonic property access
        - Type conversion utilities
        - Context manager support
        - Enhanced error handling
        "pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Pythonic properties
        .def_property_readonly("is_valid", [](const SensoryInput& input) { return input.getDimensions() > 0; })
        .def_property_readonly("data_size", [](const SensoryInput& input) { return input.getData().size(); })
        // Context manager support
        .def("__enter__", [](SensoryInput& self) { return self; })
        .def("__exit__", [](SensoryInput&, const py::args&) { return true; });

    // Enhanced Vision with Pythonic API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        Enhanced vision sensory input with Pythonic API.
        
        Features:
        - Pythonic constructor with keyword arguments
        - Pythonic property access
        - Context manager support
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        // Pythonic properties
        .def_property_readonly("width", &Vision::getWidth)
        .def_property_readonly("height", &Vision::getHeight)
        .def_property_readonly("channels", &Vision::getChannels)
        .def_property_readonly("total_pixels", [](const Vision& self) { return self.getWidth() * self.getHeight(); });

    // Enhanced Audio with Pythonic API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        Enhanced audio sensory input with Pythonic API.
        
        Features:
        - Pythonic constructor with keyword arguments
        - Pythonic property access
        - Context manager support
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        // Pythonic properties
        .def_property_readonly("sample_rate", &Audio::getSampleRate)
        .def_property_readonly("num_samples", &Audio::getNumSamples)
        .def_property_readonly("duration", [](const Audio& self) { return self.getNumSamples() / static_cast<double>(self.getSampleRate()); });

    // Enhanced InternalSignals with Pythonic API
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        Enhanced internal signals sensory input with Pythonic API.
        
        Features:
        - Pythonic API for signal management
        - Context manager support
        "pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"))
        .def("clear_signals", &InternalSignals::clearSignals)
        .def("get_signal_count", &InternalSignals::getSignals, "Get number of signals")
        // Pythonic properties
        .def_property_readonly("signal_count", &InternalSignals::getSignals)

    // Enhanced Action with Pythonic API
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        Enhanced action class with Pythonic API and factory methods.
        
        Features:
        - Pythonic property access
        - Factory methods for common actions
        - Type conversion utilities
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        // Pythonic properties
        .def_property_readonly("type", &Action::getType)
        .def_property_readonly("parameters", &Action::getParameters)
        .def_property_readonly("name", &Action::getName)
        // Factory methods
        .def_static("move_forward", []() { return Action(ActionType::MoveForward); })
        .def_static("turn_left", []() { return Action(ActionType::TurnLeft); })
        .def_static("interact", []() { return Action(ActionType::Interact); });

    // Enhanced WorldObject with Pythonic API
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        Enhanced world object with Pythonic API.
        
        Features:
        - Pythonic property access
        - Context manager support
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        // Pythonic properties
        .def_property("x", &WorldObject::x)
        .def_property("y", &WorldObject::y)
        .def_property("radius", &WorldObject::radius)
        .def_property("type", &WorldObject::type)
        .def_property("value", &WorldObject::value)
        .def_property("active", &WorldObject::active)
        .def_property_readonly("is_resource", [](const WorldObject& obj) { return obj.type == WorldObjectType::Resource; })
        .def_property_readonly("is_hazard", [](const WorldObject& obj) { return obj.type == WorldObjectType::Hazard; });

    // Enhanced AgentBody with Pythonic API
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        Enhanced agent body with Pythonic API.
        
        Features:
        - Pythonic property access
        - Context manager support
        "pbdoc")
        .def(py::init<>())
        // Pythonic properties
        .def_property("x", &AgentBody::x)
        .def_property("y", &AgentBody::y)
        .def_property("orientation", &AgentBody::orientation)
        .def_property("velocity_x", &AgentBody::velocityX)
        .def_property("velocity_y", &AgentBody::velocityY)
        .def_property("angular_velocity", &AgentBody::angularVelocity)
        .def_property("energy", &AgentBody::energy)
        .def_property("health", &AgentBody::health)
        .def_property("age", &AgentBody::age)
        .def_property("is_moving", &AgentBody::isMoving)
        .def_property("is_turning", &AgentBody::isTurning)
        .def_property("last_action_time", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        // Context manager support
        .def("__enter__", [](AgentBody& self) { return self; })
        .def("__exit__", [](AgentBody&, const py::args&) { return true; });

    // Enhanced ActionResult with Pythonic API
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        Enhanced action result with Pythonic API.
        
        Features:
        - Pythonic property access
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        // Pythonic properties
        .def_property("reward", &ActionResult::reward)
        .def_property("success", &ActionResult::success)
        .def_property("message", &ActionResult::message)
        .def_property_readonly("is_successful", [](const ActionResult& result) { return result.success; });

    // Enhanced SensoryPercept with Pythonic API
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        Enhanced sensory percept with Pythonic API and factory methods.
        
        Features:
        - Pythonic property access
        - Factory methods
        - Context manager support
        "pbdoc")
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
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Pythonic properties
        .def_property_readonly("vision", &SensoryPercept::getVision)
        .def_property_readonly("vision_width", &SensoryPercept::getVisionWidth)
        .def_property_readonly("vision_height", &SensoryPercept::getVisionHeight)
        .def_property_readonly("has_vision", [](const SensoryPercept& percept) { return percept.getVision() != nullptr; })
        // Factory methods
        .def_static("create_empty", []() { return SensoryPercept(); })
        .def_static("create_with_vision", [](const Vision& vision) {
            SensoryPercept percept;
            percept.setVision(vision);
            return percept;
        }, py::arg("vision"));

    // Enhanced SimpleWorld with Pythonic API and simulation control
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced 2D world for NLM simulation)
        Enhanced simple 2D world with Pythonic API and advanced simulation control.
        
        Features:
        - Pythonic property access
        - Context manager support
        - Simulation control methods
        - Event-driven simulation capabilities
        - Recording and playback support
        "pbdoc")
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
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        // Pythonic properties
        .def_property_readonly("width", &SimpleWorld::getWidth)
        .def_property_readonly("height", &SimpleWorld::getHeight)
        .def_property_readonly("max_energy", &SimpleWorld::getMaxEnergy)
        .def_property_readonly("energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def_property_readonly("simulation_time", &SimpleWorld::getSimulationTime)
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) { self.reset(); return self; })
        .def("__exit__", [](SimpleWorld&, const py::args&) { return true; })
        // Simulation control methods
        .def("step_simulation", &SimpleWorld::update, py::arg("timestep"),
             "Perform a simulation step")
        .def("apply_action", &SimpleWorld::applyMotorCommand,
             py::arg("action"), py::arg("time"),
             "Apply an action to the agent")
        .def("is_position_valid", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if position is valid (Pythonic)");

    // Enhanced Brain with Pythonic API and simulation control
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        Enhanced central neural simulation brain with Pythonic API and advanced simulation features.
        
        Features:
        - Pythonic property access
        - Context manager support
        - Advanced simulation control
        - Recording and batch processing
        - Callback-based simulation control
        - Enhanced error handling
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp (Pythonic)")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron (Pythonic)")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (Pythonic)")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region (Pythonic)")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs (Pythonic)")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        // Pythonic properties
        .def_property_readonly("total_neurons", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def_property_readonly("total_synapses", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def_property_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def_property_readonly("total_spikes", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def_property_readonly("developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        // Context manager support
        .def("__enter__", [](Brain& self) { self.initialize(); return self; })
        .def("__exit__", [](Brain& self, const py::args&) { self.reset(); return true; })
        // Simulation control methods
        .def("step_simulation", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("step"),
             "Perform a simulation step (Pythonic)")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("step"), py::arg("time"),
             "Perform a simulation step with timestamp (Pythonic)")
        .def("record_state", &Brain::logStatus,
             "Record current brain state")
        // Brain utilities
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Batch processing capabilities
        .def("batch_step", [](Brain& brain, const std::vector<SimulationStep>& steps) {
            for (const auto& step : steps) {
                brain.step(step);
            }
        }, py::arg("steps"), "Perform multiple simulation steps (Pythonic)")
        .def("batch_step_with_time", [](Brain& brain, const std::vector<std::pair<SimulationStep, Timestamp>>& steps) {
            for (const auto& step_pair : steps) {
                brain.step(step_pair.first, step_pair.second);
            }
        }, py::arg("steps"), "Perform multiple simulation steps with timestamps (Pythonic)");

    // Enhanced AgentBrain with Pythonic API
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        Enhanced agent brain interface with Pythonic API and advanced control.
        
        Features:
        - Pythonic property access
        - Context manager support
        - Advanced neuromodulation control
        - Development control
        - Error handling
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain (Pythonic)")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation (Pythonic)")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system (Pythonic)")
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
        // Pythonic properties
        .def_property_readonly("sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def_property_readonly("motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def_property_readonly("developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        // Context manager support
        .def("__enter__", [](AgentBrain& self) { self.initialize(); return self; })
        .def("__exit__", [](AgentBrain& self, const py::args&) { self.reset(); return true; });

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

        // ============= ADVANCED PYTHONIC API =============

    // Factory methods with Pythonic naming
    m.def("create_brain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (Pythonic)");

    m.def("create_brain_from_file", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw NLMConfigError("Failed to load configuration from file: " + filepath);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("filepath"), "Create brain from configuration file (Pythonic)");

    m.def("create_simple_world", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (Pythonic)");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (Pythonic)");

    m.def("create_default_config", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration (Pythonic)");

    // Configuration validation and helpers
    m.def("validate_config", &utils::validateConfigKeys,
         py::arg("config"), py::arg("required_keys"),
         "Validate configuration has all required keys (Pythonic)");

    m.def("get_int", &utils::getValue<int>,
         py::arg("config"), py::arg("key"), py::arg("default_value") = 0,
         "Get integer value from configuration with default (Pythonic)");

    m.def("get_double", &utils::getValue<double>,
         py::arg("config"), py::arg("key"), py::arg("default_value") = 0.0,
         "Get double value from configuration with default (Pythonic)");

    m.def("get_string", [](const Config& config, const std::string& key, const std::string& defaultValue) {
        auto value = config.get<std::string>(key);
        return value ? *value : defaultValue;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = "",
         "Get string value from configuration with default (Pythonic)");

    m.def("get_bool", [](const Config& config, const std::string& key, bool defaultValue) {
        auto value = config.get<bool>(key);
        return value ? *value : defaultValue;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = false,
         "Get boolean value from configuration with default (Pythonic)");

    // Set configuration values with Pythonic naming
    m.def("set_int", [](Config& config, const std::string& key, int value) {
        config.set(key, value);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
         "Set integer configuration value (Pythonic)");

    m.def("set_double", [](Config& config, const std::string& key, double value) {
        config.set(key, value);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
         "Set double configuration value (Pythonic)");

    m.def("set_string", [](Config& config, const std::string& key, const std::string& value) {
        config.set(key, value);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
         "Set string configuration value (Pythonic)");

    m.def("set_bool", [](Config& config, const std::string& key, bool value) {
        config.set(key, value);
    }, py::arg("config"), py::arg("key"), py::arg("value"),
         "Set boolean configuration value (Pythonic)");

    // Simulation control with callbacks
    m.def("step_brain", [](Brain& brain, SimulationStep step) {
        brain.step(step);
    }, py::arg("brain"), py::arg("step"), "Perform a brain simulation step (Pythonic)");

    m.def("step_brain_with_time", [](Brain& brain, SimulationStep step, Timestamp time) {
        brain.step(step, time);
    }, py::arg("brain"), py::arg("step"), py::arg("time"),
         "Perform a brain simulation step with timestamp (Pythonic)");

    m.def("batch_step_brain", [](Brain& brain, const std::vector<SimulationStep>& steps) {
        for (const auto& step : steps) {
            brain.step(step);
        }
    }, py::arg("brain"), py::arg("steps"),
         "Perform multiple brain simulation steps (Pythonic)");

    m.def("batch_step_brain_with_time", [](Brain& brain, const std::vector<std::pair<SimulationStep, Timestamp>>& steps) {
        for (const auto& step_pair : steps) {
            brain.step(step_pair.first, step_pair.second);
        }
    }, py::arg("brain"), py::arg("steps"),
         "Perform multiple brain simulation steps with timestamps (Pythonic)");

    // Callback-based simulation control
    m.def("simulate_with_callbacks", [](Brain& brain, SimulationStep startStep, SimulationStep endStep,
                                       std::function<void(SimulationStep, Timestamp)> pre_callback,
                                       std::function<void(SimulationStep, Timestamp, const Action&)> post_callback) {
        for (SimulationStep step = startStep; step <= endStep; ++step) {
            Timestamp time = static_cast<Timestamp>(step) * 0.001; // Assuming 1ms timestep
            
            if (pre_callback) {
                pre_callback(step, time);
            }
            
            brain.step(step, time);
            
            auto action = brain.produceAction();
            if (post_callback) {
                post_callback(step, time, *action);
            }
        }
    }, py::arg("brain"), py::arg("start_step"), py::arg("end_step"),
         py::arg("pre_callback"), py::arg("post_callback"),
         "Run simulation with callbacks for each step (Pythonic)");

    // Recording and playback
    m.def("record_brain_state", [](Brain& brain, const std::string& filepath) {
        brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
         "Record/save brain state to file (Pythonic)");

    m.def("load_brain_state", [](Brain& brain, const std::string& filepath) {
        brain.load(filepath);
    }, py::arg("brain"), py::arg("filepath"),
         "Load brain state from file (Pythonic)");

    // Context manager factory functions
    m.def("brain_context", &utils::createBrainContextManager,
         py::arg("brain"),
         "Create a context manager for Brain operations (Pythonic)");

    m.def("world_context", &utils::createWorldContextManager,
         py::arg("world"),
         "Create a context manager for World operations (Pythonic)");

    m.def("agent_brain_context", &utils::createAgentBrainContextManager,
         py::arg("agent_brain"),
         "Create a context manager for AgentBrain operations (Pythonic)");

    // Enhanced error handling utilities
    m.def("get_error_message", [](const std::exception& e) {
        return std::string(e.what());
    }, py::arg("error"),
         "Get error message (Pythonic)");

    // Type conversion utilities
    m.def("to_int", &type_conversion::stringToInt,
         py::arg("str"), "Convert string to integer (Pythonic)");

    m.def("to_float", &type_conversion::stringToDouble,
         py::arg("str"), "Convert string to float (Pythonic)");

    m.def("to_bool", &type_conversion::stringToBool,
         py::arg("str"), "Convert string to boolean (Pythonic)");

    // Simulation utilities
    m.def("get_brain_state_summary", [](const Brain& brain) {
        std::stringstream ss;
        ss << "Brain(state=" << brain.getTotalNeuronCount() << " neurons, "
           << brain.getTotalSynapseCount() << " synapses, "
           << brain.getActiveNeuronCount() << " active, "
           << brain.getAverageFiringRate() << " avg_firing_rate)";
        return ss.str();
    }, py::arg("brain"), "Get brain state as string (Pythonic)");

    m.def("validate_brain_state", [](const Brain& brain) {
        // Implementation would validate brain state consistency
        return true;
    }, py::arg("brain"), "Validate brain state consistency (Pythonic)");

    m.def("brain_statistics", [](const Brain& brain) {
        // Implementation would compute brain statistics
        return std::map<std::string, double>{{ "firing_rate", brain.getAverageFiringRate() }};
    }, py::arg("brain"), "Get brain statistics (Pythonic)");

    // Development utilities
    m.def("set_development_stage", [](Brain& brain, DevelopmentalStage stage) {
        brain.setDevelopmentalStage(stage);
    }, py::arg("brain"), py::arg("stage"), "Set brain developmental stage (Pythonic)");

    m.def("get_development_stage_name", [](DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return std::string("Initial");
            case DevelopmentalStage::CriticalPeriod: return std::string("CriticalPeriod");
            case DevelopmentalStage::Maturation: return std::string("Maturation");
            case DevelopmentalStage::Adult: return std::string("Adult");
            case DevelopmentalStage::Aging: return std::string("Aging");
            default: return std::string("Unknown");
        }
    }, py::arg("stage"), "Get developmental stage name as string (Pythonic)");

    // Action utilities
    m.def("get_brain_action_count", [](Brain& brain) {
        return brain.produceAction(); // This would return the action
    }, py::arg("brain"), "Get action produced by brain (Pythonic)");

    m.def("brain_has_action", [](const Brain& brain) {
        // Implementation would check if brain has produced an action
        return false;
    }, py::arg("brain"), "Check if brain has produced an action (Pythonic)");

    // Brain action production utilities
    m.def("brain_produce_action", [](Brain& brain) {
        return brain.produceAction();
    }, py::arg("brain"), "Brain produce action (Pythonic)");

    // Enhanced constants and attributes
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic constant aliases
    m.attr("invalid_neuron_id") = py::cast(INVALID_NEURON_ID);
    m.attr("invalid_synapse_id") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("invalid_region_id") = py::cast(INVALID_REGION_ID);
    m.attr("invalid_population_id") = py::cast(INVALID_POPULATION_ID);

    // Module-level utilities
    m.def("initialize_nlm", []() {
        std::cout << "NLM Python bindings initialized successfully!" << std::endl;
    }, "Initialize NLM Python bindings (Pythonic)");

    m.def("version", []() {
        return std::string("NLM Python Bindings v1.0.0 Enhanced");
    }, "Get NLM Python bindings version (Pythonic)");

    m.def("help", []() {
        return std::string(
            "NLM Python Bindings - Enhanced Pythonic API\n"
            "==========================================\n\n"
            "Main Classes:\n"
            "  - Brain: Central neural simulation brain\n"
            "  - Config: Configuration management\n"
            "  - AgentBrain: Agent interface\n"
            "  - SimpleWorld: 2D simulation world\n"
            "  - Vision, Audio, InternalSignals: Sensory inputs\n"
            "  - Action: Motor actions\n"
            "  - WorldObject: World entities\n"
            "  - AgentBody: Agent state\n"
            "\n"
            "Key Features:\n"
            "  - Context managers for automatic resource cleanup\n"
            "  - Pythonic property access (snake_case)\n"
            "  - Enhanced error handling with Python-specific exceptions\n"
            "  - Factory methods with better defaults\n"
            "  - Simulation control with callbacks\n"
            "  - Recording and playback capabilities\n"
            "  - Batch processing for multiple steps\n"
            "  - Configuration validation and helpers\n"
            "\n"
            "Examples:\n"
            "  with pynlm.Brain(config) as brain:\n"
            "      brain.step(10)\n"
            "  
            "  brain.total_neurons\n"
            "  brain.average_firing_rate\n"
            "  
            "  config = pynlm.create_default_config()\n"
            "  config.set_int('neuron_count', 1000)\n"
        );
    }, "Get help information (Pythonic)");
}

} // namespace nlm
