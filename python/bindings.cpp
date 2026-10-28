#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace py = pybind11;
namespace nlm {

// Python exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError(msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError(msg) {}
};

// Register Python exception types
inline void register_py_exceptions(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError.ptr());
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError.ptr());
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError.ptr());
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError.ptr());
}

// Helper function to convert time_t to Python datetime
inline py::object time_to_datetime(time_t t) {
    return py::cast(std::chrono::system_clock::from_time_t(t));
}

// Helper function to convert float vector to numpy array
inline py::object vector_to_numpy(const std::vector<float>& vec) {
    py::module_ np = py::module_::import("numpy");
    return np.attr("array")(vec);
}

// Helper function to convert numpy array to float vector
inline std::vector<float> numpy_to_vector(py::object arr) {
    py::module_ np = py::module_::import("numpy");
    py::object np_array = np.attr("asarray")(arr);
    return np_array.attr("tolist").attr("__iter__").attr("__next__")();
}

// Validation functions
inline void validate_positive(int value, const std::string& name) {
    if (value <= 0) {
        throw ValidationError(name + " must be positive, got " + std::to_string(value));
    }
}

inline void validate_range(float value, float min, float max, const std::string& name) {
    if (value < min || value > max) {
        throw ValidationError(name + " must be between " + std::to_string(min) + 
                            " and " + std::to_string(max) + ", got " + std::to_string(value));
    }
}

inline void validate_non_empty(const std::string& value, const std::string& name) {
    if (value.empty()) {
        throw ValidationError(name + " cannot be empty");
    }
}

// Context manager for Brain initialization
class BrainContext {
private:
    std::shared_ptr<Brain> brain_;
    bool initialized_;

public:
    explicit BrainContext(std::shared_ptr<Brain> brain) : brain_(brain), initialized_(false) {}

    void __enter__() {
        if (!brain_) {
            throw NLMError("Brain is null");
        }
        brain_->initialize();
        initialized_ = true;
        return *this;
    }

    void __exit__(int /*type*/, py::object /*value*/, py::object /*traceback*/) {
        if (initialized_) {
            brain_->reset();
            initialized_ = false;
        }
    }

    std::shared_ptr<Brain>& get_brain() { return brain_; }
    bool is_initialized() const { return initialized_; }
};

PYBIND11_MODULE(pynlm, m) {
    // Module version
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";

    // Module docstring with comprehensive information
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================

        A comprehensive Python API for the NLM C++ neural simulation framework.
        Provides brain simulation, neural networks, and cognitive modeling capabilities.

        Features:
        - Brain simulation with spiking neurons
        - Neural plasticity and learning
        - Developmental stages and maturation
        - Neuromodulation (dopamine, curiosity, novelty)
        - Memory systems (working, episodic, associative)
        - Prediction and planning systems
        - Cognitive mechanisms (attention, concept formation)
        - World simulation and agent interfaces
        - Checkpoint persistence and replay

        Installation: pip install pynlm
        Documentation: https://nlm.readthedocs.io
        Repository: https://github.com/nlm-project/nlm

        Quick Start:
            import pynlm
            brain = pynlm.create_brain(pynlm.create_default_config())
            brain.initialize()
            brain.step(0)
        )pbdoc";

    // Register Python exception types
    register_py_exceptions(m);

    // NeuronId class with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the index of this neuron ID")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId class with enhanced functionality
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the index of this synapse ID")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId class with enhanced functionality
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the index of this region ID")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId class with enhanced functionality
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the index of this population ID")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
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

    // Config class with enhanced functionality
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Enhanced config methods
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            validate_non_empty(key, "Key");
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration string value")
        .def("set", [](Config& self, const std::string& key, int value) {
            validate_non_empty(key, "Key");
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration integer value")
        .def("set", [](Config& self, const std::string& key, double value) {
            validate_non_empty(key, "Key");
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration float value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            validate_non_empty(key, "Key");
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration boolean value")
        .def("get", [](const Config& self, const std::string& key) {
            validate_non_empty(key, "Key");
            return self.get(key);
        }, py::arg("key"),
           "Get a configuration value by key")
        .def("get_string", [](const Config& self, const std::string& key) {
            validate_non_empty(key, "Key");
            return self.get(key);
        }, py::arg("key"),
           "Get a configuration string value by key")
        .def("get_int", [](const Config& self, const std::string& key) {
            validate_non_empty(key, "Key");
            return self.get(key);
        }, py::arg("key"),
           "Get a configuration integer value by key")
        .def("get_float", [](const Config& self, const std::string& key) {
            validate_non_empty(key, "Key");
            return self.get(key);
        }, py::arg("key"),
           "Get a configuration float value by key")
        .def("get_bool", [](const Config& self, const std::string& key) {
            validate_non_empty(key, "Key");
            return self.get(key);
        }, py::arg("key"),
           "Get a configuration boolean value by key");

    // SensoryInput class with enhanced functionality
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_numpy", &vector_to_numpy, "Convert data to NumPy array")
        .def("__repr__", [](const SensoryInput& input) {
            std::stringstream ss;
            ss << "<SensoryInput type=" << static_cast<int>(input.getType())
               << " dimensions=" << input.getDimensions() << ">";
            return ss.str();
        });

    // Vision class with enhanced functionality
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            validate_positive(data.size(), "Data");
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("set_data_from_numpy", [](Vision& self, py::object numpy_array) {
            std::vector<float> data = numpy_to_vector(numpy_array);
            self.setData(data);
        }, py::arg("numpy_array"), "Set vision data from NumPy array")
        .def("get_width", &Vision::getWidth, "Get vision width")
        .def("get_height", &Vision::getHeight, "Get vision height")
        .def("get_channels", &Vision::getChannels, "Get vision channels")
        .def("get_data_flat", [](const Vision& self) {
            return vector_to_numpy(self.getData());
        }, "Get vision data as NumPy array");

    // Audio class with enhanced functionality
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            validate_positive(data.size(), "Data");
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("set_data_from_numpy", [](Audio& self, py::object numpy_array) {
            std::vector<float> data = numpy_to_vector(numpy_array);
            self.setData(data);
        }, py::arg("numpy_array"), "Set audio data from NumPy array")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set sample rate")
        .def("get_sample_rate", &Audio::getSampleRate, "Get sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get number of samples")
        .def("get_data_flat", [](const Audio& self) {
            return vector_to_numpy(self.getData());
        }, "Get audio data as NumPy array");

    // InternalSignals class with enhanced functionality
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("get_signals", &InternalSignals::getSignals, "Get all signals")
        .def("get_signal_count", &InternalSignals::getSignalCount, "Get signal count")
        .def("get_data_flat", [](const InternalSignals& self) {
            return vector_to_numpy(self.getData());
        }, "Get signals as NumPy array");

    // Action class with enhanced functionality
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType, "Get action type")
        .def("set_type", &Action::setType, py::arg("type"), "Set action type")
        .def("get_parameters", &Action::getParameters, "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("get_name", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Clone action")
        .def("to_dict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            result["name"] = action.getName();
            return result;
        }, "Convert action to dictionary")
        .def("from_dict", [](Action& action, py::dict dict) {
            auto it_type = dict.find("type");
            auto it_params = dict.find("parameters");
            auto it_name = dict.find("name");
            
            if (it_type != dict.end()) {
                action.setType(py::cast<ActionType>(it_type->second));
            }
            if (it_params != dict.end()) {
                action.setParameters(py::cast<std::vector<float>>(it_params->second));
            }
            if (it_name != dict.end()) {
                // Action doesn't have setName method, so we'll skip this
                // action.setName(py::cast<std::string>(it_name->second));
            }
        }, py::arg("dict"), "Update action from dictionary");

    // WorldObject struct with enhanced functionality
    py::struct_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("is_near", [](const WorldObject& obj, float x, float y, float threshold) {
            return obj.distance_to(x, y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"),
           "Check if object is near position")
        .def("to_dict", [](const WorldObject& obj) {
            py::dict result;
            result["x"] = obj.x;
            result["y"] = obj.y;
            result["radius"] = obj.radius;
            result["type"] = obj.type;
            result["value"] = obj.value;
            result["active"] = obj.active;
            return result;
        }, "Convert world object to dictionary")
        .def("from_dict", [](WorldObject& obj, py::dict dict) {
            auto it_x = dict.find("x");
            auto it_y = dict.find("y");
            auto it_radius = dict.find("radius");
            auto it_type = dict.find("type");
            auto it_value = dict.find("value");
            auto it_active = dict.find("active");
            
            if (it_x != dict.end()) obj.x = py::cast<float>(it_x->second);
            if (it_y != dict.end()) obj.y = py::cast<float>(it_y->second);
            if (it_radius != dict.end()) obj.radius = py::cast<float>(it_radius->second);
            if (it_type != dict.end()) obj.type = py::cast<WorldObjectType>(it_type->second);
            if (it_value != dict.end()) obj.value = py::cast<float>(it_value->second);
            if (it_active != dict.end()) obj.active = py::cast<bool>(it_active->second);
        }, py::arg("dict"), "Update world object from dictionary");

    // AgentBody class with enhanced functionality
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (radians)")
        .def_readwrite("velocity_x", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocity_y", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age (simulation steps)")
        .def_readwrite("is_moving", &AgentBody::isMoving, "Moving state")
        .def_readwrite("is_turning", &AgentBody::isTurning, "Turning state")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        .def("speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Calculate speed")
        .def("get_position", [](const AgentBody& body) {
            return py::make_tuple(body.x, body.y);
        }, "Get position as tuple (x, y)")
        .def("set_position", [](AgentBody& body, float x, float y) {
            validate_range(x, -1000.0f, 1000.0f, "X position");
            validate_range(y, -1000.0f, 1000.0f, "Y position");
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"), "Set position");

    // ActionResult class with enhanced functionality
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success state")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("is_positive", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if result is positive")
        .def("is_negative", [](const ActionResult& result) {
            return result.reward < 0.0f;
        }, "Check if result is negative")
        .def("to_dict", [](const ActionResult& result) {
            py::dict d;
            d["reward"] = result.reward;
            d["success"] = result.success;
            d["message"] = result.message;
            return d;
        }, "Convert action result to dictionary")
        .def("from_dict", [](ActionResult& result, py::dict d) {
            auto it_reward = d.find("reward");
            auto it_success = d.find("success");
            auto it_message = d.find("message");
            
            if (it_reward != d.end()) result.reward = py::cast<float>(it_reward->second);
            if (it_success != d.end()) result.success = py::cast<bool>(it_success->second);
            if (it_message != d.end()) result.message = py::cast<std::string>(it_message->second);
        }, py::arg("dict"), "Update action result from dictionary");

    // SensoryPercept class with enhanced functionality
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision, "Get vision data")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch data")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio data")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals, "Get all signals")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict d;
            d["vision"] = percept.getVision();
            d["touch"] = percept.getTouch();
            d["internal"] = percept.getInternal();
            d["proprioception"] = percept.getProprioception();
            d["audio"] = percept.getAudio();
            d["timestamp"] = percept.getTimestamp();
            return d;
        }, "Convert sensory percept to dictionary")
        .def("from_dict", [](SensoryPercept& percept, py::dict d) {
            auto it_vision = d.find("vision");
            auto it_touch = d.find("touch");
            auto it_internal = d.find("internal");
            auto it_proprioception = d.find("proprioception");
            auto it_audio = d.find("audio");
            auto it_timestamp = d.find("timestamp");
            
            if (it_vision != d.end()) percept.setVision(py::cast<Vision>(it_vision->second));
            if (it_touch != d.end()) percept.setTouch(py::cast<std::vector<float>>(it_touch->second));
            if (it_internal != d.end()) percept.setInternal(py::cast<InternalSignals>(it_internal->second));
            if (it_proprioception != d.end()) percept.setProprioception(py::cast<std::vector<float>>(it_proprioception->second));
            if (it_audio != d.end()) percept.setAudio(py::cast<Audio>(it_audio->second));
            if (it_timestamp != d.end()) percept.setTimestamp(py::cast<time_t>(it_timestamp->second));
        }, py::arg("dict"), "Update sensory percept from dictionary")
        .def("has_vision", [](const SensoryPercept& percept) {
            return !percept.getVision().getData().empty();
        }, "Check if vision data is available")
        .def("has_audio", [](const SensoryPercept& percept) {
            return !percept.getAudio().getData().empty();
        }, "Check if audio data is available")
        .def("has_internal", [](const SensoryPercept& percept) {
            return !percept.getInternal().getData().empty();
        }, "Check if internal signals are available");

    // SimpleWorld class with enhanced functionality
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate,
             py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        // Enhanced world methods
        .def("create_agent", [](SimpleWorld& world, float x, float y) {
            AgentBody agent;
            agent.x = x;
            agent.y = y;
            agent.orientation = 0.0f;
            agent.velocityX = 0.0f;
            agent.velocityY = 0.0f;
            agent.angularVelocity = 0.0f;
            agent.energy = world.getMaxEnergy();
            agent.health = 100.0f;
            agent.age = 0.0f;
            agent.isMoving = false;
            agent.isTurning = false;
            agent.lastActionTime = world.getSimulationTime();
            return agent;
        }, py::arg("x"), py::arg("y"), "Create agent body at position")
        .def("create_resource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            world.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Create resource object")
        .def("create_hazard", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value, 0.3f);
            world.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Create hazard object")
        .def("get_objects_by_type", [](SimpleWorld& world, WorldObjectType type) {
            py::list result;
            // Note: This would need access to internal world objects
            // For now, returning empty list as a placeholder
            return result;
        }, py::arg("type"), "Get all objects of specified type")
        .def("get_nearby_objects", [](SimpleWorld& world, float x, float y, float radius) {
            py::list result;
            // Note: This would need access to internal world objects
            // For now, returning empty list as a placeholder
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get objects near position");

    // Brain class with enhanced functionality
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
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
        .def("reset", &Brain::reset, "Reset brain state")
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
        .def("log_status", &Brain::logStatus, "Log brain status")
        // Enhanced brain methods
        .def("inject_current_to_population", [](Brain& brain, PopulationId population, float current) {
            brain.injectCurrentToNeurons(population, current);
        }, py::arg("population"), py::arg("current"),
           "Inject current into all neurons of a population")
        .def("inject_current_to_type", [](Brain& brain, NeuronType type, float current) {
            brain.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
           "Inject current into all neurons of a type")
        .def("produce_action_with_sensory", [](Brain& brain, const SensoryInput& input) {
            brain.receiveSensoryInput(input);
            return brain.produceAction();
        }, py::arg("input"), "Produce action with sensory input")
        .def("get_statistics", [](Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["spike_count"] = brain.getTotalSpikeCount();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["developmental_stage"] = brain.getDevelopmentalStage();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("set_config_value", [](Brain& brain, const std::string& key, const std::string& value) {
            validate_non_empty(key, "Key");
            brain.getConfig().set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value")
        .def("set_config_value_int", [](Brain& brain, const std::string& key, int value) {
            validate_non_empty(key, "Key");
            brain.getConfig().set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration integer value")
        .def("set_config_value_float", [](Brain& brain, const std::string& key, float value) {
            validate_non_empty(key, "Key");
            brain.getConfig().set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration float value")
        .def("set_config_value_bool", [](Brain& brain, const std::string& key, bool value) {
            validate_non_empty(key, "Key");
            brain.getConfig().set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration boolean value")
        .def("get_config_value", [](Brain& brain, const std::string& key) {
            validate_non_empty(key, "Key");
            return brain.getConfig().get(key);
        }, py::arg("key"), "Get configuration value")
        .def("get_config_value_string", [](Brain& brain, const std::string& key) {
            validate_non_empty(key, "Key");
            return brain.getConfig().get(key);
        }, py::arg("key"), "Get configuration string value")
        .def("get_config_value_int", [](Brain& brain, const std::string& key) {
            validate_non_empty(key, "Key");
            return brain.getConfig().get(key);
        }, py::arg("key"), "Get configuration integer value")
        .def("get_config_value_float", [](Brain& brain, const std::string& key) {
            validate_non_empty(key, "Key");
            return brain.getConfig().get(key);
        }, py::arg("key"), "Get configuration float value")
        .def("get_config_value_bool", [](Brain& brain, const std::string& key) {
            validate_non_empty(key, "Key");
            return brain.getConfig().get(key);
        }, py::arg("key"), "Get configuration boolean value");

    // AgentBrain class with enhanced functionality
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
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
        // Enhanced agent brain methods
        .def("step", [](AgentBrain& agent, SimpleWorld& world, float timestep) {
            agent.updateDevelopment(timestep);
            return agent.getDevelopmentalStage();
        }, py::arg("world"), py::arg("timestep"),
           "Update development system with world")
        .def("process_step", [](AgentBrain& agent, SimpleWorld& world, float timestep) {
            auto percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            return agent.decodeMotorCommand();
        }, py::arg("world"), py::arg("timestep"),
           "Process a simulation step")
        .def("get_behavior_stats", [](AgentBrain& agent) {
            py::dict stats;
            stats["developmental_stage"] = agent.getDevelopmentalStage();
            stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
            stats["curiosity_level"] = agent.getCuriosityLevel();
            stats["novelty_level"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent.isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent.isCuriosityEnabled();
            return stats;
        }, "Get agent brain behavior statistics")
        .def("create_simple_episode", [](AgentBrain& agent, SimpleWorld& world, int steps) {
            for (int i = 0; i < steps; ++i) {
                world.update(0.1f);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                auto action = agent.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                agent.updateDevelopment(0.1f);
            }
        }, py::arg("world"), py::arg("steps"), "Create a simple episode");

    // Context manager for Brain
    py::class_<BrainContext>(m, "BrainContext", R"pbdoc(Context manager for brain initialization and cleanup)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", &BrainContext::__enter__)
        .def("__exit__", &BrainContext::__exit__)
        .def("get_brain", &BrainContext::get_brain, "Get the brain instance")
        .def("is_initialized", &BrainContext::is_initialized, "Check if brain is initialized");

    // Module functions with enhanced convenience
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw ValidationError("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);  // Default world size
        return world;
    }, "Create a default simple world")
    .def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw ValidationError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    // Enhanced module functions
    .def("create_brain_with_config", [](const std::string& config_file) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(config_file);
        return std::make_shared<Brain>(config);
    }, py::arg("config_file"), "Create brain from config file")
    .def("create_brain_with_defaults", [](int neuron_count, float connection_density) -> std::shared_ptr<Brain> {
        validate_positive(neuron_count, "Neuron count");
        validate_range(connection_density, 0.0f, 1.0f, "Connection density");
        
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuron_count);
        config->set("brain.synapse_density", connection_density);
        return std::make_shared<Brain>(config);
    }, py::arg("neuron_count") = 1000, py::arg("connection_density") = 0.1,
       "Create brain with default configuration")
    .def("run_simulation", [](std::shared_ptr<Brain> brain, int steps) {
        if (!brain) {
            throw ValidationError("Brain cannot be null");
        }
        validate_positive(steps, "Number of steps");
        
        brain->initialize();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"),
       "Run a complete brain simulation and return spike count")
    .def("create_simple_agent_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, int steps) {
        if (!brain || !world) {
            throw ValidationError("Brain and world cannot be null");
        }
        validate_positive(steps, "Number of steps");
        
        auto config = std::make_shared<Config>();
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            agent->updateDevelopment(0.1f);
        }
        
        return agent->getCuriosityLevel();
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
       "Create a simple agent episode and return final curiosity level")
    // Constants and utilities
    .def("create_brain_context", [](std::shared_ptr<Brain> brain) {
        return BrainContext(brain);
    }, py::arg("brain"), "Create a context manager for brain operations")
    .def("get_constants", []() {
        py::dict constants;
        constants["invalid_neuron_id"] = INVALID_NEURON_ID;
        constants["invalid_synapse_id"] = INVALID_SYNAPSE_ID;
        constants["invalid_region_id"] = INVALID_REGION_ID;
        constants["invalid_population_id"] = INVALID_POPULATION_ID;
        constants["default_neuron_count"] = 1000;
        constants["default_synapse_density"] = 0.1f;
        constants["max_simulation_time"] = 1000.0f;
        return constants;
    }, "Get NLM constants")
    // Invalid IDs (for convenience)
    .attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID)
    .attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID)
    .attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID)
    .attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Helper functions
    m.def("validate_positive", &validate_positive,
          py::arg("value"), py::arg("name"),
          "Validate that a value is positive")
    .def("validate_range", &validate_range,
          py::arg("value"), py::arg("min"), py::arg("max"), py::arg("name"),
          "Validate that a value is within a range")
    .def("validate_non_empty", &validate_non_empty,
          py::arg("value"), py::arg("name"),
          "Validate that a string is not empty")
    .def("time_to_datetime", &time_to_datetime,
          py::arg("time_t"), "Convert time_t to Python datetime")
    .def("vector_to_numpy", &vector_to_numpy,
          py::arg("vector"), "Convert vector to NumPy array")
    .def("numpy_to_vector", &numpy_to_vector,
          py::arg("array"), "Convert NumPy array to vector");

    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";

    // Type aliases for convenience
    py::class_<SimulationStep>(m, "SimulationStep")
        .def(py::init<>())  // Default constructor
        .def(py::init<int64_t>(), py::arg("step"))
        .def_readwrite("step", &SimulationStep::step)
        .def("__repr__", [](const SimulationStep& s) {
            return "<SimulationStep: " + std::to_string(s.step) + ">";
        });

    py::class_<Timestamp>(m, "Timestamp")
        .def(py::init<>())  // Default constructor
        .def(py::init<double>(), py::arg("time"))
        .def_readwrite("time", &Timestamp::time)
        .def("__repr__", [](const Timestamp& t) {
            return "<Timestamp: " + std::to_string(t.time) + ">";
        });
} // namespace nlm