#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

// Helper function to convert vector to string
template<typename T>
std::string vectorToString(const std::vector<T>& vec) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << vec[i];
    }
    ss << "]";
    return ss.str();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Advanced features:
        - Factory methods for common configurations
        - Convenience wrappers for frequent operations
        - Enhanced visualization support
        - Better error handling and type safety
    )pbdoc";

    // Custom exception types with better error messages
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Enhanced ID classes with Pythonic interfaces
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const std::string&>(), py::arg("value"),
             "Create NeuronId from string representation")
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__str__", [](const NeuronId& id) { return std::to_string(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def_static("from_string", [](const std::string& s) {
            try { return NeuronId(std::stoull(s)); } catch (...) {
                throw py::value_error("Invalid NeuronId format: " + s);
            }
        }, py::arg("value"), "Create NeuronId from string");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const std::string&>(), py::arg("value"),
             "Create SynapseId from string representation")
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__str__", [](const SynapseId& id) { return std::to_string(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def_static("from_string", [](const std::string& s) {
            try { return SynapseId(std::stoull(s)); } catch (...) {
                throw py::value_error("Invalid SynapseId format: " + s);
            }
        }, py::arg("value"), "Create SynapseId from string");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const std::string&>(), py::arg("value"),
             "Create RegionId from string representation")
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__str__", [](const RegionId& id) { return std::to_string(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def_static("from_string", [](const std::string& s) {
            try { return RegionId(std::stoull(s)); } catch (...) {
                throw py::value_error("Invalid RegionId format: " + s);
            }
        }, py::arg("value"), "Create RegionId from string");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const std::string&>(), py::arg("value"),
             "Create PopulationId from string representation")
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__str__", [](const PopulationId& id) { return std::to_string(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def_static("from_string", [](const std::string& s) {
            try { return PopulationId(std::stoull(s)); } catch (...) {
                throw py::value_error("Invalid PopulationId format: " + s);
            }
        }, py::arg("value"), "Create PopulationId from string");

    // Enhanced enumerations with Pythonic access patterns
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_excitatory", [](NeuronType type) { return type == NeuronType::Excitatory; })
        .def("is_inhibitory", [](NeuronType type) { return type == NeuronType::Inhibitory; });

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_excitatory", [](SynapseType type) { return type == SynapseType::Excitatory; })
        .def("is_inhibitory", [](SynapseType type) { return type == SynapseType::Inhibitory; });

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_developing", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Initial || 
                   stage == DevelopmentalStage::CriticalPeriod ||
                   stage == DevelopmentalStage::Maturation;
        })
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Aging;
        });

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("is_active", [](FiringState state) { return state == FiringState::Active; })
        .def("is_refractory", [](FiringState state) { return state == FiringState::Refractory; });

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
        .def("is_movement", [](ActionType action) {
            return action == ActionType::MoveForward || action == ActionType::MoveBackward ||
                   action == ActionType::MoveLeft || action == ActionType::MoveRight;
        })
        .def("is_rotation", [](ActionType action) {
            return action == ActionType::TurnLeft || action == ActionType::TurnRight;
        });

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
        .def("is_movement", [](MotorCommand cmd) {
            return cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward;
        });

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("is_hazard", [](WorldObjectType type) {
            return type == WorldObjectType::Hazard || type == WorldObjectType::Wall;
        })
        .def("is_resource", [](WorldObjectType type) {
            return type == WorldObjectType::Resource || type == WorldObjectType::Marker;
        });

    // Enhanced Config class with Python-friendly API
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
        .def("get", [](Config& self, const std::string& key) {
            try {
                // Try different types for flexibility
                if (self.has(key + "_int64")) {
                    return self.getOr<int64_t>(key + "_int64", 0);
                } else if (self.has(key + "_float")) {
                    return self.getOr<float>(key + "_float", 0.0f);
                } else if (self.has(key + "_double")) {
                    return self.getOr<double>(key + "_double", 0.0);
                } else if (self.has(key + "_string")) {
                    return self.getOr<std::string>(key + "_string", "");
                } else {
                    return self.getOr<int64_t>(key, 0);
                }
            } catch (...) {
                return self.getOr<int64_t>(key, 0);
            }
        }, py::arg("key"), "Get configuration value (flexible type access)")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value (string)")
        .def("set_int", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value (int)")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"), "Set configuration value (float)")
        .def("get_neurons", [](const Config& self) {
            return self.getOr<int64_t>("neuron_count", 500);
        }, "Get number of neurons (convenience method)")
        .def("get_regions", [](const Config& self) {
            return self.getOr<int64_t>("region_count", 1);
        }, "Get number of regions (convenience method)")
        .def("get_timestep", [](const Config& self) {
            return self.getOr<double>("simulation_timestep", 0.001);
        }, "Get simulation timestep (convenience method)")
        .def("validate", [](const Config& self) {
            std::vector<std::string> errors;
            // Basic validation
            if (self.has("neuron_count") && self.getOr<int64_t>("neuron_count", 0) <= 0) {
                errors.push_back("neuron_count must be positive");
            }
            if (self.has("simulation_timestep") && self.getOr<double>("simulation_timestep", 0) <= 0) {
                errors.push_back("simulation_timestep must be positive");
            }
            if (self.has("region_count") && self.getOr<int64_t>("region_count", 0) <= 0) {
                errors.push_back("region_count must be positive");
            }
            if (!errors.empty()) {
                throw py::value_error("Configuration validation failed: " + 
                                    vectorToString(errors));
            }
            return true;
        }, "Validate configuration (throws on error)");

    // Enhanced SensoryInput class with Python-friendly API
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) +
                   " dim=" + std::to_string(input.getDimensions()) + ">";
        });

    // Enhanced Vision class with Python-friendly API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from vector of floats")
        .def("setData", [](Vision& self, const py::array_t<float>& data) {
            // Convert numpy array to vector
            py::buffer_info buf = data.request();
            float* ptr = (float*) buf.ptr;
            size_t size = buf.size;
            std::vector<float> vec(ptr, ptr + size);
            self.setData(vec);
        }, py::arg("data"), "Set vision data from numpy array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("create_blank", [](size_t width, size_t height, size_t channels = 3) {
            Vision vision(width, height, channels);
            std::vector<float> blank(width * height * channels, 0.0f);
            vision.setData(blank);
            return vision;
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
            "Create a blank vision input")
        .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
            // Note: This would require actual resizing implementation
            throw py::not_implemented_error("Vision resize not yet implemented");
        }, py::arg("width"), py::arg("height"), "Resize vision input");

    // Enhanced Audio class with Python-friendly API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from vector of floats")
        .def("setData", [](Audio& self, const py::array_t<float>& data) {
            // Convert numpy array to vector
            py::buffer_info buf = data.request();
            float* ptr = (float*) buf.ptr;
            size_t size = buf.size;
            std::vector<float> vec(ptr, ptr + size);
            self.setData(vec);
        }, py::arg("data"), "Set audio data from numpy array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("create_silence", [](size_t sampleRate, size_t duration_ms) {
            size_t numSamples = (sampleRate * duration_ms) / 1000;
            Audio audio(sampleRate, numSamples);
            std::vector<float> silence(numSamples, 0.0f);
            audio.setData(silence);
            return audio;
        }, py::arg("sampleRate"), py::arg("duration_ms"), "Create silent audio input");

    // Enhanced InternalSignals class with Python-friendly API
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignal", [](InternalSignals& self, const std::vector<float>& signals) {
            for (float signal : signals) {
                self.addSignal(signal);
            }
        }, py::arg("signals"), "Add multiple signals")
        .def("getAllSignals", &InternalSignals::getAllSignals)
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getAllSignals)
        .def("getMean", [](const InternalSignals& self) {
            const auto& signals = self.getAllSignals();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, "Get mean of all signals")
        .def("getMax", [](const InternalSignals& self) {
            const auto& signals = self.getAllSignals();
            if (signals.empty()) return 0.0f;
            return *std::max_element(signals.begin(), signals.end());
        }, "Get maximum signal value")
        .def("getMin", [](const InternalSignals& self) {
            const auto& signals = self.getAllSignals();
            if (signals.empty()) return 0.0f;
            return *std::min_element(signals.begin(), signals.end());
        }, "Get minimum signal value");

    // Enhanced Action class with Python-friendly API
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
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at index")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to Python dictionary")
        .def_static("from_dict", [](const py::dict& data) {
            Action action;
            if (data.contains("type")) {
                action.setType(static_cast<ActionType>(py::cast<int>(data["type"])));
            }
            if (data.contains("parameters")) {
                action.setParameters(py::cast<std::vector<float>>(data["parameters"]));
            }
            return action;
        }, py::arg("data"), "Create action from Python dictionary")
        .def("__repr__", [](const Action& action) {
            std::stringstream ss;
            ss << "<Action type=" << static_cast<int>(action.getType()) 
               << " params=" << vectorToString(action.getParameters()) << ">";
            return ss.str();
        });

    // Enhanced WorldObject class with Python-friendly API
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
        .def("is_hazard", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Hazard || obj.type == WorldObjectType::Wall;
        }, "Check if object is hazardous")
        .def("is_resource", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Resource || obj.type == WorldObjectType::Marker;
        }, "Check if object is a resource")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("is_near", [](const WorldObject& self, float x, float y, float distance) {
            return self.distance_to(x, y) <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if object is near point")
        .def("__repr__", [](const WorldObject& obj) {
            std::stringstream ss;
            ss << "<WorldObject x=" << obj.x << " y=" << obj.y 
               << " type=" << static_cast<int>(obj.type) << " active=" << obj.active << ">";
            return ss.str();
        });

    // Enhanced AgentBody class with Python-friendly API
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
        .def("reset", &AgentBody::reset)
        .def("distance_to", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("is_near", [](const AgentBody& self, float x, float y, float distance) {
            return self.distance_to(x, y) <= distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"), "Check if agent is near point")
        .def("to_dict", [](const AgentBody& body) {
            py::dict result;
            result["x"] = body.x;
            result["y"] = body.y;
            result["orientation"] = body.orientation;
            result["velocityX"] = body.velocityX;
            result["velocityY"] = body.velocityY;
            result["angularVelocity"] = body.angularVelocity;
            result["energy"] = body.energy;
            result["health"] = body.health;
            result["age"] = body.age;
            result["isMoving"] = body.isMoving;
            result["isTurning"] = body.isTurning;
            result["lastActionTime"] = body.lastActionTime;
            return result;
        }, "Convert agent body to Python dictionary")
        .def_static("from_dict", [](const py::dict& data) {
            AgentBody body;
            if (data.contains("x")) body.x = py::cast<float>(data["x"]);
            if (data.contains("y")) body.y = py::cast<float>(data["y"]);
            if (data.contains("orientation")) body.orientation = py::cast<float>(data["orientation"]);
            if (data.contains("velocityX")) body.velocityX = py::cast<float>(data["velocityX"]);
            if (data.contains("velocityY")) body.velocityY = py::cast<float>(data["velocityY"]);
            if (data.contains("angularVelocity")) body.angularVelocity = py::cast<float>(data["angularVelocity"]);
            if (data.contains("energy")) body.energy = py::cast<float>(data["energy"]);
            if (data.contains("health")) body.health = py::cast<float>(data["health"]);
            if (data.contains("age")) body.age = py::cast<float>(data["age"]);
            if (data.contains("isMoving")) body.isMoving = py::cast<bool>(data["isMoving"]);
            if (data.contains("isTurning")) body.isTurning = py::cast<bool>(data["isTurning"]);
            if (data.contains("lastActionTime")) body.lastActionTime = py::cast<float>(data["lastActionTime"]);
            return body;
        }, py::arg("data"), "Create agent body from Python dictionary")
        .def("__repr__", [](const AgentBody& body) {
            std::stringstream ss;
            ss << "<AgentBody x=" << body.x << " y=" << body.y 
               << " health=" << body.health << ">";
            return ss.str();
        });

    // Enhanced ActionResult class with Python-friendly API
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_good", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if action was successful and rewarding")
        .def("to_dict", [](const ActionResult& result) {
            py::dict d;
            d["reward"] = result.reward;
            d["success"] = result.success;
            d["message"] = result.message;
            return d;
        }, "Convert action result to Python dictionary")
        .def_static("from_dict", [](const py::dict& data) {
            ActionResult result;
            if (data.contains("reward")) result.reward = py::cast<float>(data["reward"]);
            if (data.contains("success")) result.success = py::cast<bool>(data["success"]);
            if (data.contains("message")) result.message = py::cast<std::string>(data["message"]);
            return result;
        }, py::arg("data"), "Create action result from Python dictionary")
        .def("__repr__", [](const ActionResult& result) {
            std::stringstream ss;
            ss << "<ActionResult reward=" << result.reward 
               << " success=" << (result.success ? "true" : "false") << ">";
            return ss.str();
        });

    // Enhanced SensoryPercept class with Python-friendly API
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
        .def("add_vision", [](SensoryPercept& self, const std::vector<float>& data, size_t width, size_t height, size_t channels = 3) {
            Vision vision(width, height, channels);
            vision.setData(data);
            self.setVision(vision);
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
            "Add vision data with dimensions")
        .def("add_touch", [](SensoryPercept& self, const std::vector<float>& data) {
            // For simplicity, assuming touch data is just a vector of floats
            auto touch = self.getTouch();
            touch.insert(touch.end(), data.begin(), data.end());
            self.setTouch(touch);
        }, py::arg("data"), "Add touch data")
        .def("add_internal", [](SensoryPercept& self, const std::vector<float>& data) {
            auto internal = self.getInternal();
            internal.insert(internal.end(), data.begin(), data.end());
            self.setInternal(internal);
        }, py::arg("data"), "Add internal signals")
        .def("add_audio", [](SensoryPercept& self, const std::vector<float>& data, size_t sampleRate) {
            Audio audio(sampleRate, data.size());
            audio.setData(data);
            self.setAudio(audio);
        }, py::arg("data"), py::arg("sampleRate"), "Add audio data with sample rate")
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            
            // Add vision data if available
            auto vision = percept.getVision();
            if (!vision.isEmpty()) {
                py::dict vision_data;
                vision_data["width"] = vision.getWidth();
                vision_data["height"] = vision.getHeight();
                vision_data["channels"] = vision.getChannels();
                vision_data["data"] = vision.getData();
                result["vision"] = vision_data;
            }
            
            // Add touch data
            auto touch = percept.getTouch();
            if (!touch.empty()) {
                result["touch"] = touch;
            }
            
            // Add internal signals
            auto internal = percept.getInternal();
            if (!internal.empty()) {
                result["internal"] = internal;
            }
            
            // Add audio data if available
            auto audio = percept.getAudio();
            if (!audio.isEmpty()) {
                py::dict audio_data;
                audio_data["sampleRate"] = audio.getSampleRate();
                audio_data["numSamples"] = audio.getNumSamples();
                audio_data["data"] = audio.getData();
                result["audio"] = audio_data;
            }
            
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert percept to Python dictionary")
        .def_static("from_dict", [](const py::dict& data) {
            SensoryPercept percept;
            
            if (data.contains("vision")) {
                auto vision_data = py::cast<py::dict>(data["vision"]);
                size_t width = py::cast<size_t>(vision_data["width"]);
                size_t height = py::cast<size_t>(vision_data["height"]);
                size_t channels = py::cast<size_t>(vision_data["channels"]);
                auto vision_data_vec = py::cast<std::vector<float>>(vision_data["data"]);
                Vision vision(width, height, channels);
                vision.setData(vision_data_vec);
                percept.setVision(vision);
            }
            
            if (data.contains("touch")) {
                percept.setTouch(py::cast<std::vector<float>>(data["touch"]));
            }
            
            if (data.contains("internal")) {
                percept.setInternal(py::cast<std::vector<float>>(data["internal"]));
            }
            
            if (data.contains("audio")) {
                auto audio_data = py::cast<py::dict>(data["audio"]);
                size_t sampleRate = py::cast<size_t>(audio_data["sampleRate"]);
                size_t numSamples = py::cast<size_t>(audio_data["numSamples"]);
                auto audio_data_vec = py::cast<std::vector<float>>(audio_data["data"]);
                Audio audio(sampleRate, numSamples);
                audio.setData(audio_data_vec);
                percept.setAudio(audio);
            }
            
            if (data.contains("timestamp")) {
                percept.setTimestamp(py::cast<float>(data["timestamp"]));
            }
            
            return percept;
        }, py::arg("data"), "Create percept from Python dictionary")
        .def("__repr__", [](const SensoryPercept& percept) {
            std::stringstream ss;
            ss << "<SensoryPercept vision=" << percept.getVisionWidth() << "x" 
               << percept.getVisionHeight() << ">";
            return ss.str();
        });

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
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
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
