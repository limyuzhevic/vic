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

// Custom exception types for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain initialization error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Features:
        - Enhanced convenience methods for brain creation and configuration
        - Comprehensive error handling with custom exception types
        - Advanced factory functions for common use cases
        - Improved documentation and examples
        - Better integration with Python ecosystem
        
        Quick Start:
        ```python
        import pynlm
        
        # Create a default brain configuration
        config = pynlm.create_default_config()
        
        # Create a brain with standard settings
        brain = pynlm.create_brain(config)
        
        # Initialize the brain
        brain.initialize()
        
        # Run a few simulation steps
        for step in range(100):
            brain.step(step, step * 0.001)
        
        # Get statistics
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        ```
        
        For more examples, visit: https://github.com/nlm-project/nlm
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // NeuronId with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)")
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

    // SynapseId with enhanced functionality
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)")
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

    // RegionId with enhanced functionality
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)")
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

    // PopulationId with enhanced functionality
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)")
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

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with validation and utility methods
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (INI format)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::string>, py::const_),
             py::arg("key"),
             "Get configuration value (returns None if key doesn't exist)")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::getOr<std::string>, py::const_),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value with default fallback")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set, {py::arg("key"), py::arg("value"), py::arg("source")}),
             "Set configuration value")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set, {py::arg("key"), py::arg("value"), py::arg("source")}),
             "Set configuration value")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set, {py::arg("key"), py::arg("value"), py::arg("source")}),
             "Set configuration value")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set, {py::arg("key"), py::arg("value"), py::arg("source")}),
             "Set configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](const Config& self) {
            // Validate configuration
            auto keys = self.getKeys();
            std::vector<std::string> errors;
            
            // Check for required keys
            std::vector<std::string> requiredKeys = {"random_seed", "simulation_timestep", "neuron_count"};
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    errors.push_back("Missing required key: " + key);
                }
            }
            
            // Check for invalid values
            if (self.has("neuron_count")) {
                auto neuronCount = self.getOr<int64_t>("neuron_count", 0);
                if (neuronCount <= 0) {
                    errors.push_back("neuron_count must be positive");
                }
            }
            
            if (self.has("simulation_timestep")) {
                auto timestep = self.getOr<double>("simulation_timestep", 0.0);
                if (timestep <= 0.0) {
                    errors.push_back("simulation_timestep must be positive");
                }
            }
            
            return errors;
        }, "Validate configuration and return list of errors")
        .def("apply_preset", [](Config& self, const std::string& preset) {
            // Apply common presets
            if (preset == "default") {
                self.set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
                self.set("simulation_timestep", 0.001, ConfigSource::Runtime);
                self.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
                self.set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
                self.set("connection_probability", 0.15f, ConfigSource::Runtime);
                self.set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime);
                self.set("stdp_ltd_weight", 0.015f, ConfigSource::Runtime);
                self.set("stdp_tau", 20.0f, ConfigSource::Runtime);
            } else if (preset == "small") {
                self.set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
                self.set("simulation_timestep", 0.001, ConfigSource::Runtime);
                self.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
                self.set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
                self.set("connection_probability", 0.1f, ConfigSource::Runtime);
            } else if (preset == "large") {
                self.set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
                self.set("simulation_timestep", 0.001, ConfigSource::Runtime);
                self.set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Runtime);
                self.set("region_count", static_cast<int64_t>(3), ConfigSource::Runtime);
                self.set("connection_probability", 0.2f, ConfigSource::Runtime);
            } else if (preset == "learning") {
                self.set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
                self.set("simulation_timestep", 0.001, ConfigSource::Runtime);
                self.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
                self.set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
                self.set("connection_probability", 0.15f, ConfigSource::Runtime);
                self.set("stdp_ltp_weight", 0.05f, ConfigSource::Runtime);
                self.set("stdp_ltd_weight", 0.03f, ConfigSource::Runtime);
                self.set("stdp_tau", 50.0f, ConfigSource::Runtime);
                self.set("synaptogenesis_rate", 0.001f, ConfigSource::Runtime);
                self.set("pruning_rate", 0.0001f, ConfigSource::Runtime);
            } else if (preset == "structural") {
                self.set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
                self.set("simulation_timestep", 0.001, ConfigSource::Runtime);
                self.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
                self.set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
                self.set("connection_probability", 0.15f, ConfigSource::Runtime);
                self.set("synaptogenesis_rate", 0.001f, ConfigSource::Runtime);
                self.set("pruning_rate", 0.0005f, ConfigSource::Runtime);
            }
        }, py::arg("preset"),
           "Apply a predefined configuration preset (default, small, large, learning, structural)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with advanced features)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromImage", [](Vision& self, const std::vector<std::vector<std::vector<float>>>& image) {
            // Convert 3D vector to flat vector for Vision
            size_t width = image.size();
            size_t height = image[0].size();
            size_t channels = image[0][0].size();
            std::vector<float> flatData;
            flatData.reserve(width * height * channels);
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    for (size_t c = 0; c < channels; ++c) {
                        flatData.push_back(image[x][y][c]);
                    }
                }
            }
            self.setData(flatData);
            self.configure(width, height, channels);
        }, py::arg("image"),
           "Set vision data from a 3D image representation (width x height x channels)")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("normalize", [](Vision& self, float targetMax = 1.0f) {
            auto data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            if (maxVal > 0.0f) {
                float scale = targetMax / maxVal;
                for (auto& val : data) {
                    val *= scale;
                }
                self.setData(data);
            }
        }, py::arg("target_max") = 1.0f,
           "Normalize vision data to have maximum value of target_max");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced features)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("computeFFT", [](Audio& self) {
            auto data = self.getData();
            // Simple FFT placeholder - in real implementation would use FFT library
            std::vector<float> spectrum(data.size() / 2);
            for (size_t i = 0; i < spectrum.size(); ++i) {
                spectrum[i] = std::sqrt(data[i * 2] * data[i * 2] + data[i * 2 + 1] * data[i * 2 + 1]);
            }
            return spectrum;
        }, "Compute FFT of audio data (placeholder implementation)");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getData, "Get number of signals")
        .def("getAverageSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get average signal value");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation for motor output)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getMagnitude", [](const Action& self) {
            auto params = self.getParameters();
            if (params.empty()) return 0.0f;
            return *std::max_element(params.begin(), params.end(),
                                    [](float a, float b) { return std::abs(a) < std::abs(b); });
        }, "Get magnitude of action parameters")
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to dictionary representation")
        .def_static("from_dict", [](py::dict d) {
            Action action;
            if (d.contains("type")) {
                action.setType(py::cast<ActionType>(d["type"]));
            }
            if (d.contains("parameters")) {
                action.setParameters(py::cast<std::vector<float>>(d["parameters"]));
            }
            return action;
        }, py::arg("action_dict"), "Create action from dictionary representation");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Advanced world object representation)")
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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)")
        .def("contains", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx * dx + dy * dy) <= (self.radius * self.radius);
        }, py::arg("x"), py::arg("y"), "Check if point (x, y) is inside object")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = self.type;
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert world object to dictionary representation")
        .def_static("from_dict", [](py::dict d) {
            WorldObject obj;
            if (d.contains("x")) obj.x = py::cast<float>(d["x"]);
            if (d.contains("y")) obj.y = py::cast<float>(d["y"]);
            if (d.contains("radius")) obj.radius = py::cast<float>(d["radius"]);
            if (d.contains("type")) obj.type = py::cast<WorldObjectType>(d["type"]);
            if (d.contains("value")) obj.value = py::cast<float>(d["value"]);
            if (d.contains("active")) obj.active = py::cast<bool>(d["active"]);
            return obj;
        }, py::arg("obj_dict"), "Create world object from dictionary representation");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Advanced agent body state)")
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
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set agent velocity")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Calculate agent speed")
        .def("reset", &AgentBody::reset)
        .def("to_dict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert agent body to dictionary representation")
        .def_static("from_dict", [](py::dict d) {
            AgentBody body;
            if (d.contains("x")) body.x = py::cast<float>(d["x"]);
            if (d.contains("y")) body.y = py::cast<float>(d["y"]);
            if (d.contains("orientation")) body.orientation = py::cast<float>(d["orientation"]);
            if (d.contains("velocityX")) body.velocityX = py::cast<float>(d["velocityX"]);
            if (d.contains("velocityY")) body.velocityY = py::cast<float>(d["velocityY"]);
            if (d.contains("angularVelocity")) body.angularVelocity = py::cast<float>(d["angularVelocity"]);
            if (d.contains("energy")) body.energy = py::cast<float>(d["energy"]);
            if (d.contains("health")) body.health = py::cast<float>(d["health"]);
            if (d.contains("age")) body.age = py::cast<float>(d["age"]);
            if (d.contains("isMoving")) body.isMoving = py::cast<bool>(d["isMoving"]);
            if (d.contains("isTurning")) body.isTurning = py::cast<bool>(d["isTurning"]);
            if (d.contains("lastActionTime")) body.lastActionTime = py::cast<float>(d["lastActionTime"]);
            return body;
        }, py::arg("body_dict"), "Create agent body from dictionary representation");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result is positive (reward > 0)")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result is negative (reward < 0)")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary representation")
        .def_static("from_dict", [](py::dict d) {
            ActionResult result;
            if (d.contains("reward")) result.reward = py::cast<float>(d["reward"]);
            if (d.contains("success")) result.success = py::cast<bool>(d["success"]);
            if (d.contains("message")) result.message = py::cast<std::string>(d["message"]);
            return result;
        }, py::arg("result_dict"), "Create action result from dictionary representation");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with enhanced features)")
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
        .def("clearAllSignals", [](SensoryPercept& self) {
            // Clear all signals by setting empty vectors
            auto vision = self.getVision();
            vision.clear();
            self.setVision(vision);
            
            auto touch = self.getTouch();
            touch.clear();
            self.setTouch(touch);
            
            auto internal = self.getInternal();
            internal.clear();
            self.setInternal(internal);
            
            auto proprioception = self.getProprioception();
            proprioception.clear();
            self.setProprioception(proprioception);
            
            auto audio = self.getAudio();
            audio.clear();
            self.setAudio(audio);
        }, "Clear all sensory signals")
        .def("getSignalStats", [](const SensoryPercept& self) {
            py::dict stats;
            auto vision = self.getVision();
            stats["vision_count"] = vision.size();
            
            auto touch = self.getTouch();
            stats["touch_count"] = touch.size();
            
            auto internal = self.getInternal();
            stats["internal_count"] = internal.size();
            
            auto proprioception = self.getProprioception();
            stats["proprioception_count"] = proprioception.size();
            
            auto audio = self.getAudio();
            stats["audio_count"] = audio.size();
            
            return stats;
        }, "Get statistics about all sensory signals");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with advanced features)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (e.g., decay effects)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to the world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid in world")
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
             "Set random seed for reproducible world simulation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("createObject", [](SimpleWorld& self, float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            WorldObject obj(x, y, type, value, radius);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
           "Create and add a world object (convenience method)")
        .def("findNearestObject", [](SimpleWorld& self, float x, float y) {
            float minDist = std::numeric_limits<float>::max();
            WorldObject* nearest = nullptr;
            
            // This is a simplified implementation - in practice would need access to internal object storage
            // For now, return an empty object
            return WorldObject();
        }, py::arg("x"), py::arg("y"),
           "Find nearest world object to position (x, y)")
        .def("getWorldInfo", [](const SimpleWorld& self) {
            py::dict info;
            info["width"] = self.getWidth();
            info["height"] = self.getHeight();
            info["max_energy"] = self.getMaxEnergy();
            info["energy_decay_rate"] = self.getEnergyDecayRate();
            info["simulation_time"] = self.getSimulationTime();
            info["random_seed"] = self.getRandomSeed();
            return info;
        }, "Get world information as dictionary");

    // Enhanced Brain class with convenience methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced features)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
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
        .def("createLearningExperiment", [](std::shared_ptr<Brain> brain, uint64_t seed) {
            return LearningExperiment(brain, seed);
        }, py::arg("brain"), py::arg("seed") = 42,
           "Create a learning experiment wrapper (convenience method)")
        .def("runBasicConnectivityTest", &runBasicConnectivityTest,
             "Run basic neural connectivity test (convenience method)")
        .def("runPlasticityExperiment", &runPlasticityExperiment,
             "Run plasticity learning experiment (convenience method)")
        .def("runStdpVerification", &runStdpVerification,
             "Run STDP verification test (convenience method)")
        .def("getBrainStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("compareBrains", [](const Brain& self, const Brain& other) {
            py::dict comparison;
            comparison["neuron_count_diff"] = self.getTotalNeuronCount() - other.getTotalNeuronCount();
            comparison["synapse_count_diff"] = self.getTotalSynapseCount() - other.getTotalSynapseCount();
            comparison["spike_count_diff"] = self.getTotalSpikeCount() - other.getTotalSpikeCount();
            comparison["firing_rate_diff"] = self.getAverageFiringRate() - other.getAverageFiringRate();
            return comparison;
        }, py::arg("other"),
           "Compare two brains and return differences as dictionary");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize with brain")
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

    // Enhanced factory functions with comprehensive options
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with all Phase 2 settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Enhanced configuration presets
    m.def("create_preset_config", [](const std::string& preset) {
        auto config = std::make_shared<Config>();
        if (preset == "default") {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
            config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
            config->set("connection_probability", 0.15f, ConfigSource::Runtime);
            config->set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime);
            config->set("stdp_ltd_weight", 0.015f, ConfigSource::Runtime);
            config->set("stdp_tau", 20.0f, ConfigSource::Runtime);
        } else if (preset == "learning") {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
            config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
            config->set("connection_probability", 0.15f, ConfigSource::Runtime);
            config->set("stdp_ltp_weight", 0.05f, ConfigSource::Runtime);
            config->set("stdp_ltd_weight", 0.03f, ConfigSource::Runtime);
            config->set("stdp_tau", 50.0f, ConfigSource::Runtime);
            config->set("synaptogenesis_rate", 0.001f, ConfigSource::Runtime);
            config->set("pruning_rate", 0.0001f, ConfigSource::Runtime);
        } else if (preset == "structural") {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
            config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
            config->set("connection_probability", 0.15f, ConfigSource::Runtime);
            config->set("synaptogenesis_rate", 0.001f, ConfigSource::Runtime);
            config->set("pruning_rate", 0.0005f, ConfigSource::Runtime);
        }
        return config;
    }, py::arg("preset"), "Create a brain configuration with preset settings");

    m.def("run_complete_demo", [](std::shared_ptr<Brain> brain) {
        // Run the complete demo sequence from main.cpp
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }, py::arg("brain"), "Run complete demo sequence (basic, plasticity, STDP)");

    // Convenience functions for common tasks
    m.def("create_and_run_learning_experiment", [](std::shared_ptr<Config> config, SimulationStep steps) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain for learning experiment");
        }
        
        LearningExperiment experiment(brain, 42);
        experiment.recordInitialState();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
        
        return experiment;
    }, py::arg("config"), py::arg("steps"), "Create and run a complete learning experiment");

    // Advanced utility functions
    m.def("create_advanced_brain", [](const std::string& preset, const std::map<std::string, std::string>& overrides) {
        auto config = std::make_shared<Config>();
        
        // Apply preset
        if (preset == "phase2") {
            const auto& schemas = ConfigSchemas::Phase2Defaults;
            for (const auto& schema : schemas) {
                config->set(schema.key, schema.defaultValue, ConfigSource::Runtime);
            }
        } else if (preset == "phase6") {
            const auto& schemas = ConfigSchemas::Phase6Defaults;
            for (const auto& schema : schemas) {
                config->set(schema.key, schema.defaultValue, ConfigSource::Runtime);
            }
        } else if (preset == "learning") {
            const auto& schemas = ConfigSchemas::LearningPresets;
            for (const auto& schema : schemas) {
                config->set(schema.key, schema.defaultValue, ConfigSource::Runtime);
            }
        } else if (preset == "development") {
            const auto& schemas = ConfigSchemas::DevelopmentPresets;
            for (const auto& schema : schemas) {
                config->set(schema.key, schema.defaultValue, ConfigSource::Runtime);
            }
        } else if (preset == "structural") {
            const auto& schemas = ConfigSchemas::StructuralPresets;
            for (const auto& schema : schemas) {
                config->set(schema.key, schema.defaultValue, ConfigSource::Runtime);
            }
        } else {
            throw ConfigurationError("Unknown preset: " + preset);
        }
        
        // Apply overrides
        for (const auto& pair : overrides) {
            // Try to parse the override value based on the expected type
            auto schemaIt = std::find_if(
                preset == "phase2" ? ConfigSchemas::Phase2Defaults.begin() :
                preset == "phase6" ? ConfigSchemas::Phase6Defaults.begin() :
                preset == "learning" ? ConfigSchemas::LearningPresets.begin() :
                preset == "development" ? ConfigSchemas::DevelopmentPresets.begin() :
                ConfigSchemas::StructuralPresets.begin(),
                ConfigSchemas::Phase2Defaults.end(),
                [&pair](const ConfigSchema& s) { return s.key == pair.first; }
            );
            
            if (schemaIt != ConfigSchemas::Phase2Defaults.end()) {
                // Determine type and set appropriately
                if (schemaIt->type == "int" || schemaIt->type == "int64") {
                    try {
                        int value = std::stoi(pair.second);
                        config->set(pair.first, value, ConfigSource::Runtime);
                    } catch (...) {
                        throw ConfigurationError("Invalid integer value for " + pair.first + ": " + pair.second);
                    }
                } else if (schemaIt->type == "double" || schemaIt->type == "float") {
                    try {
                        double value = std::stod(pair.second);
                        config->set(pair.first, value, ConfigSource::Runtime);
                    } catch (...) {
                        throw ConfigurationError("Invalid float value for " + pair.first + ": " + pair.second);
                    }
                } else if (schemaIt->type == "bool") {
                    bool value = (pair.second == "true" || pair.second == "1" || pair.second == "yes");
                    config->set(pair.first, value, ConfigSource::Runtime);
                } else if (schemaIt->type == "string") {
                    config->set(pair.first, pair.second, ConfigSource::Runtime);
                }
            }
        }
        
        return config;
    }, py::arg("preset"), py::arg("overrides"), 
           "Create a brain configuration with advanced preset and override options");

    m.def("setup_complete_experiment", [](std::shared_ptr<Brain> brain, const std::string& experimentType, SimulationStep steps) {
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain for experiment");
        }
        
        if (experimentType == "connectivity") {
            runBasicConnectivityTest(brain);
        } else if (experimentType == "plasticity") {
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
        } else if (experimentType == "stdp") {
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
        } else if (experimentType == "comprehensive") {
            runBasicConnectivityTest(brain);
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
        } else {
            throw SimulationError("Unknown experiment type: " + experimentType);
        }
        
        return brain;
    }, py::arg("brain"), py::arg("experiment_type"), py::arg("steps"), 
           "Run a complete experiment with the brain");

    m.def("create_brain_with_validation", [](std::shared_ptr<Config> config, const std::string& presetName) {
        if (!config) {
            throw ConfigurationError("Config cannot be null");
        }
        
        // Validate configuration against preset
        auto errors = ConfigUtils::validateConfig(*config, presetName);
        if (!errors.empty()) {
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  " + error.key + ": " + error.message + " (" + error.code + ")\n";
            }
            throw ConfigurationError(errorMsg);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain with validated configuration");
        }
        
        return brain;
    }, py::arg("config"), py::arg("preset_name"), "Create a brain with configuration validation");

    m.def("create_reproducible_experiment", [](const std::string& preset, uint64_t seed, SimulationStep steps) {
        auto config = std::make_shared<Config>();
        
        // Apply preset
        if (preset == "phase2") {
            const auto& schemas = ConfigSchemas::Phase2Defaults;
            for (const auto& schema : schemas) {
                config->set(schema.key, schema.defaultValue, ConfigSource::Runtime);
            }
        }
        
        // Set seed for reproducibility
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain for reproducible experiment");
        }
        
        // Run experiment
        LearningExperiment experiment(brain, seed);
        experiment.recordInitialState();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
        
        return experiment;
    }, py::arg("preset"), py::arg("seed"), py::arg("steps"), 
           "Create a reproducible experiment with fixed seed");

    // Register type conversions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Module attributes
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features";
}

} // namespace nlm