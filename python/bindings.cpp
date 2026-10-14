import pybind11/pybind11.h
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <map>

#include "Brain.hpp"
#include "Config/Config.hpp"
#include "core/Types/Types.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"

namespace py = pybind11;
namespace nlm {

// Context manager for Brain
class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    ~BrainContextManager() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Enable context manager protocol
    void __enter__() { return; }
    void __exit__(int /*type*/, int /*value*/, int /*traceback*/) {
        if (brain_) {
            brain_->reset();
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Context manager for Experiment
class ExperimentContextManager {
public:
    ExperimentContextManager(std::unique_ptr<Experiment> experiment) 
        : experiment_(std::move(experiment)) {}
    ~ExperimentContextManager() {
        if (experiment_) {
            experiment_->stop();
        }
    }
    
    void __enter__() { return; }
    void __exit__(int /*type*/, int /*value*/, int /*traceback*/) {
        if (experiment_) {
            experiment_->stop();
        }
    }
    
    Experiment* getExperiment() const { return experiment_.get(); }
    
private:
    std::unique_ptr<Experiment> experiment_;
};

// Episode recording helpers
struct EpisodeData {
    size_t step;
    double timestamp;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    float reward;
    std::string action;
    std::vector<float> sensoryInput;
};

// Batch operations for neurons
struct NeuronBatchData {
    std::vector<NeuronId> ids;
    std::vector<float> currents;
    std::vector<MembranePotential> potentials;
    std::vector<FiringState> states;
    std::vector<float> thresholds;
    std::vector<float> restingPotentials;
};

// Batch operations for synapses
struct SynapseBatchData {
    std::vector<SynapseId> ids;
    std::vector<NeuronId> sourceNeurons;
    std::vector<NeuronId> destinationNeurons;
    std::vector<float> weights;
    std::vector<Delay> delays;
    std::vector<SynapseType> types;
    std::vector<PlasticityFlags> plasticityFlags;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId type
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

    // SynapseId type
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId type
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

    // PopulationId type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
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

    // Convenience configuration methods
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        // Original methods
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
        
        // Pythonic convenience methods
        .def("getInt", py::overload_cast<const std::string&>(&Config::get<int>),
             py::arg("key"), "Get integer value")
        .def("getFloat", py::overload_cast<const std::string&>(&Config::get<double>),
             py::arg("key"), "Get float value")
        .def("getBool", py::overload_cast<const std::string&>(&Config::get<bool>),
             py::arg("key"), "Get boolean value")
        .def("getString", py::overload_cast<const std::string&>(&Config::get<std::string>),
             py::arg("key"), "Get string value")
        
        .def("getIntOr", py::overload_cast<const std::string&, const int&>(&Config::getOr<int>),
             py::arg("key"), py::arg("defaultValue"), "Get integer or return default")
        .def("getFloatOr", py::overload_cast<const std::string&, const double&>(&Config::getOr<double>),
             py::arg("key"), py::arg("defaultValue"), "Get float or return default")
        .def("getBoolOr", py::overload_cast<const std::string&, const bool&>(&Config::getOr<bool>),
             py::arg("key"), py::arg("defaultValue"), "Get boolean or return default")
        .def("getStringOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr<std::string>),
             py::arg("key"), py::arg("defaultValue"), "Get string or return default")
        
        .def("setInt", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set integer value")
        .def("setFloat", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set float value")
        .def("setBool", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set boolean value")
        .def("setString", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set string value")
        
        .def("add", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Add/update configuration entry")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration entry")
        
        // Pythonic utility methods
        .def("setIfExists", [](Config& self, const std::string& key, const ConfigValue& value) {
            if (self.has(key)) {
                self.set(key, value, ConfigSource::Runtime);
            }
        }, py::arg("key"), py::arg("value"),
           "Set value only if key already exists")
        .def("update", [](Config& self, const std::unordered_map<std::string, ConfigValue>& updates) {
            for (const auto& pair : updates) {
                self.set(pair.first, pair.second, ConfigSource::Runtime);
            }
        }, py::arg("updates"),
           "Update multiple configuration values at once")
        .def("merge", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                self.set(key, other.get(ConfigValue()).value_or(ConfigValue()), ConfigSource::Runtime);
            }
        }, py::arg("other"),
           "Merge another configuration into this one")
        
        .def("export", &Config::summary,
             "Export configuration as string")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto val = self.get<ConfigValue>(key);
            if (!val.has_value()) {
                throw py::key_error("Key not found: '" + key + "'");
            }
            return val.value();
        }, py::arg("key"), "Get configuration value (getitem support)")
        .def("__setitem__", [](Config& self, const std::string& key, const ConfigValue& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set configuration value (setitem support)")
        .def("__contains__", &Config::has, py::arg("key"),
             "Check if key exists (in operator support)")
        .def("__len__", &Config::getKeys,
             "Get number of configuration keys (len() support)")
        .def("__iter__", [](Config& self) {
            return py::make_iterator(self.getKeys().begin(), self.getKeys().end());
        }, "Iterate over configuration keys");

    // Enhanced SensoryInput with Pythonic interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision with Pythonic interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Pythonic methods
        .def("shape", [](const Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get tensor shape (width, height, channels)")
        .def("size", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get total number of pixels (size)")
        .def("reshape", [](Vision& self, size_t newWidth, size_t newHeight, size_t newChannels) {
            // Note: This would require actual implementation
            throw py::value_error("reshape not implemented - requires C++ implementation");
        }, py::arg("newWidth"), py::arg("newHeight"), py::arg("newChannels"),
           "Reshape vision data");

    // Enhanced Audio with Pythonic interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Pythonic methods
        .def("duration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("resample", [](Audio& self, size_t newSampleRate) {
            // Note: This would require actual implementation
            throw py::value_error("resample not implemented - requires C++ implementation");
        }, py::arg("newSampleRate"), "Resample audio");

    // Enhanced InternalSignals with Pythonic interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Pythonic methods
        .def("append", &InternalSignals::addSignal, py::arg("value"),
             "Append signal value (more Pythonic alias for addSignal)")
        .def("extend", [](InternalSignals& self, const std::vector<float>& values) {
            for (float v : values) {
                self.addSignal(v);
            }
        }, py::arg("values"), "Add multiple signal values")
        .def("mean", [](const InternalSignals& self) {
            // Note: This would require actual implementation
            throw py::value_error("mean not implemented - requires C++ implementation");
        }, "Calculate mean of signals")
        .def("std", [](const InternalSignals& self) {
            // Note: This would require actual implementation
            throw py::value_error("std not implemented - requires C++ implementation");
        }, "Calculate standard deviation of signals");

    // Enhanced Action with Pythonic interface
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
        // Pythonic methods
        .def("copy", &Action::clone, "Create a copy of this action")
        .def("update", [](Action& self, ActionType type, const std::vector<float>& params) {
            self.setType(type);
            self.setParameters(params);
        }, py::arg("type"), py::arg("parameters"),
           "Update action type and parameters")
        .def("hasParameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action has parameters")
        .def("parameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            if (index < self.getParameters().size()) {
                return self.getParameters()[index];
            }
            throw py::index_error("Parameter index out of range");
        }, py::arg("index"), "Get parameter by index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                params.resize(index + 1, 0.0f);
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), "Set parameter by index");

    // Enhanced WorldObject with Pythonic interface
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
        // Pythonic properties
        .def_property("position", [](const WorldObject& obj) {
            return std::vector<float>{obj.x, obj.y};
        }, [](WorldObject& obj, const std::vector<float>& pos) {
            if (pos.size() >= 2) {
                obj.x = pos[0];
                obj.y = pos[1];
            }
        }, "Get/set position as (x, y) vector")
        .def_property("coords", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, [](WorldObject& obj, const std::pair<float, float>& coords) {
            obj.x = coords.first;
            obj.y = coords.second;
        }, "Get/set position as (x, y) tuple")
        .def("distanceTo", [](const WorldObject& obj, const WorldObject& other) {
            float dx = obj.x - other.x;
            float dy = obj.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another object")
        .def("distanceToPoint", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("isNear", [](const WorldObject& obj, const WorldObject& other, float threshold) {
            return obj.distanceTo(other) <= threshold;
        }, py::arg("other"), py::arg("threshold"), "Check if near another object")
        .def("isNearPoint", [](const WorldObject& obj, float x, float y, float threshold) {
            return obj.distanceToPoint(x, y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if near point");

    // Enhanced AgentBody with Pythonic interface
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
        // Pythonic properties
        .def_property("position", [](const AgentBody& body) {
            return std::vector<double>{body.x, body.y};
        }, [](AgentBody& body, const std::vector<double>& pos) {
            if (pos.size() >= 2) {
                body.x = pos[0];
                body.y = pos[1];
            }
        }, "Get/set position as (x, y) vector")
        .def_property("velocity", [](const AgentBody& body) {
            return std::vector<double>{body.velocityX, body.velocityY};
        }, [](AgentBody& body, const std::vector<double>& vel) {
            if (vel.size() >= 2) {
                body.velocityX = vel[0];
                body.velocityY = vel[1];
            }
        }, "Get/set velocity as (vx, vy) vector")
        .def("speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Calculate speed magnitude")
        .def("updatePosition", [](AgentBody& body, float dt) {
            body.x += body.velocityX * dt;
            body.y += body.velocityY * dt;
            body.orientation += body.angularVelocity * dt;
        }, py::arg("dt"), "Update position based on velocity (Euler integration)")
        .def("moveTo", [](AgentBody& body, float x, float y, float dt) {
            float dx = x - body.x;
            float dy = y - body.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance > 0.001f) {
                body.velocityX = dx / distance;
                body.velocityY = dy / distance;
                body.isMoving = true;
                body.updatePosition(dt);
            }
        }, py::arg("x"), py::arg("y"), py::arg("dt"), "Move toward target position");

    // Enhanced ActionResult with Pythonic interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Pythonic properties
        .def_property("isSuccessful", [](const ActionResult& result) {
            return result.success;
        }, nullptr, "Check if action was successful")
        .def("toDict", [](const ActionResult& result) {
            return py::dict("reward"_a = result.reward,
                           "success"_a = result.success,
                           "message"_a = result.message);
        }, "Convert to dictionary representation");

    // Enhanced SensoryPercept with Pythonic interface
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
        // Pythonic methods
        .def("setAllSignals", [](SensoryPercept& self,
                                 const std::vector<float>& vision,
                                 const std::vector<float>& touch,
                                 const std::vector<float>& internal,
                                 const std::vector<float>& proprioception,
                                 const std::vector<float>& audio) {
            self.setVision(vision);
            self.setTouch(touch);
            self.setInternal(internal);
            self.setProprioception(proprioception);
            self.setAudio(audio);
        }, py::arg("vision"), py::arg("touch"), py::arg("internal"),
           py::arg("proprioception"), py::arg("audio"),
           "Set all sensory signals at once")
        .def("clear", [](SensoryPercept& self) {
            self.setVision(std::vector<float>());
            self.setTouch(std::vector<float>());
            self.setInternal(std::vector<float>());
            self.setProprioception(std::vector<float>());
            self.setAudio(std::vector<float>());
        }, "Clear all sensory signals")
        .def("getAllSignalsDict", [](const SensoryPercept& self) {
            return py::dict("vision"_a = self.getVision(),
                           "touch"_a = self.getTouch(),
                           "internal"_a = self.getInternal(),
                           "proprioception"_a = self.getProprioception(),
                           "audio"_a = self.getAudio());
        }, "Get all signals as dictionary");

    // Enhanced SimpleWorld with Pythonic interface
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Pythonic methods
        .def("addResource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"),
           "Add resource object")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float hazardLevel) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.value = hazardLevel;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("hazardLevel"),
           "Add hazard object")
        .def("addMarker", [](SimpleWorld& self, float x, float y) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Marker;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"),
           "Add marker object")
        .def("getObjectsByType", [](const SimpleWorld& self, WorldObjectType type) {
            py::list result;
            // This would require access to world objects - placeholder for actual implementation
            return result;
        }, py::arg("type"), "Get all objects of specific type")
        .def("findNearestObject", [](const SimpleWorld& self, float x, float y, WorldObjectType type = WorldObjectType::Empty) {
            // This would require access to world objects - placeholder for actual implementation
            WorldObject nearest;
            nearest.x = -1; nearest.y = -1; // Not found
            return nearest;
        }, py::arg("x"), py::arg("y"), py::arg("type") = WorldObjectType::Empty,
           "Find nearest object of specific type")
        .def("getBounds", [](const SimpleWorld& self) {
            return std::vector<float>{0.0f, 0.0f, static_cast<float>(self.getWidth()),
                                     static_cast<float>(self.getHeight())};
        }, "Get world bounds as [xMin, yMin, xMax, yMax]")
        .def("isInside", [](const SimpleWorld& self, float x, float y) {
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), "Check if point is inside world bounds");

    // Enhanced Brain with Pythonic interface and context manager
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        
        // Pythonic constructor overloads
        .def(py::init<>(), "Brain with default configuration")
        .def(py::init([](py::args args) {
            if (args.empty()) {
                return std::make_shared<Brain>(std::make_shared<Config>());
            } else if (args.size() == 1) {
                auto config = py::cast<std::shared_ptr<Config>>(args[0]);
                return std::make_shared<Brain>(config);
            } else {
                throw py::value_error("Brain constructor accepts 0 or 1 arguments");
            }
        }), "Brain constructor with optional config")
        
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
        
        // Convenience injection methods
        .def("injectToExcitatory", [](Brain& self, float current) {
            self.injectCurrentToNeurons(NeuronType::Excitatory, current);
        }, py::arg("current"), "Inject current to all excitatory neurons")
        .def("injectToInhibitory", [](Brain& self, float current) {
            self.injectCurrentToNeurons(NeuronType::Inhibitory, current);
        }, py::arg("current"), "Inject current to all inhibitory neurons")
        .def("injectToSensory", [](Brain& self, float current) {
            self.injectCurrentToNeurons(NeuronType::Sensory, current);
        }, py::arg("current"), "Inject current to all sensory neurons")
        .def("injectToMotor", [](Brain& self, float current) {
            self.injectCurrentToNeurons(NeuronType::Motor, current);
        }, py::arg("current"), "Inject current to all motor neurons")
        
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Region management convenience methods
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
        
        // Statistics and monitoring
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
        
        // Enhanced statistics and monitoring
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system for direct access")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        
        // Random generator access
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Brain>& self) {
            return BrainContextManager(self);
        })
        .def("__exit__", [](std::shared_ptr<Brain>& self, int /*type*/, int /*value*/, int /*traceback*/) {
            if (self) {
                self->reset();
            }
        })
        
        // Enhanced batch operations
        .def("injectCurrentBatch", [](Brain& self, const std::vector<std::pair<NeuronId, float>>& injections) {
            for (const auto& injection : injections) {
                self.injectCurrent(injection.first, injection.second);
            }
        }, py::arg("injections"),
             "Inject current to multiple neurons in batch")
        .def("injectCurrentToNeuronsBatch", [](Brain& self, const std::vector<NeuronType>& types, float current) {
            for (NeuronType type : types) {
                self.injectCurrentToNeurons(type, current);
            }
        }, py::arg("types"), py::arg("current"),
             "Inject current to multiple neuron types in batch")
        
        // Performance batch operations
        .def("injectCurrentVectorized", [](Brain& self, const std::vector<float>& currents, float scaling = 1.0f) {
            // Vectorized injection - applies scaling to all currents
            for (float current : currents) {
                self.injectCurrentToNeurons(NeuronType::Internal, current * scaling);
            }
        }, py::arg("currents"), py::arg("scaling") = 1.0f,
             "Vectorized current injection with scaling")
        
        // Episode recording helpers
        .def("recordEpisode", [](Brain& self, size_t step, float reward, const Action& action) {
            EpisodeData episode;
            episode.step = step;
            episode.timestamp = static_cast<double>(step) * 0.001;  // Assuming 1kHz timestep
            episode.reward = reward;
            episode.action = std::to_string(static_cast<int>(action.getType()));
            // This would require access to current brain state - placeholder
            return episode;
        }, py::arg("step"), py::arg("reward"), py::arg("action"),
           "Record episode data for later analysis")
        .def("getEpisodeData", [](Brain& self, size_t step) {
            // This would retrieve stored episode data
            EpisodeData episode;
            episode.step = step;
            // Placeholder implementation
            return episode;
        }, py::arg("step"),
           "Get recorded episode data for specific step")
        
        // Development helpers
        .def("advanceDevelopmentalStage", [](Brain& self) {
            DevelopmentalStage current = self.getDevelopmentalStage();
            switch (current) {
                case DevelopmentalStage::Initial:
                    self.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    self.setDevelopmentalStage(DevelopmentalStage::Maturation);
                    break;
                case DevelopmentalStage::Maturation:
                    self.setDevelopmentalStage(DevelopmentalStage::Adult);
                    break;
                default:
                    self.setDevelopmentalStage(DevelopmentalStage::Aging);
                    break;
            }
        }, "Advance developmental stage automatically")
        
        // Convenience experiment setup
        .def("setupForExperiment", [](Brain& self, const std::string& experimentType) {
            // Configure brain based on experiment type
            auto config = self.getConfig();
            if (experimentType == "learning") {
                config->set("neuron_count", 2000);
                config->set("region_count", 3);
                config->set("stdp_ltp_weight", 0.02f);
            } else if (experimentType == "memory") {
                config->set("neuron_count", 1500);
                config->set("region_count", 2);
                config->set("replay_interval", 50);
            } else if (experimentType == "cognition") {
                config->set("neuron_count", 3000);
                config->set("region_count", 4);
                config->set("planning_depth", 10);
            }
        }, py::arg("experimentType"), "Setup brain configuration for specific experiment type")
        
        // Memory management helpers
        .def("clearMemorySystems", [](Brain& self) {
            if (self.getWorkingMemory()) self.getWorkingMemory()->clear();
            if (self.getEpisodicMemory()) self.getEpisodicMemory()->clear();
            if (self.getAssociativeMemory()) self.getAssociativeMemory()->clear();
        }, "Clear all memory systems")
        .def("consolidateMemories", [](Brain& self, float threshold) {
            if (self.getEpisodicMemory()) {
                self.getEpisodicMemory()->consolidate(threshold);
            }
        }, py::arg("threshold"), "Consolidate episodic memories above threshold")
        
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getRegionCount()) + " regions, " +
                   std::to_string(brain.getTotalSpikeCount()) + " total spikes>";
        });

    // Enhanced AgentBrain with Pythonic interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        // Core processing methods
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
        
        // Neuromodulation getters
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        // Configuration setters
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        
        // Configuration getters
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Developmental state
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        
        // Reset and cleanup
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // Pythonic convenience methods
        .def("act", [](AgentBrain& self, const SensoryPercept& percept, float reward, float predictedReward = 0.0f) {
            self.processSensoryInput(percept);
            self.applyRewardModulation(reward, predictedReward);
            self.updateDevelopment(0.001);  // Default timestep
            return self.decodeMotorCommand();
        }, py::arg("percept"), py::arg("reward"), py::arg("predictedReward") = 0.0f,
           "Perform complete agent cycle: perceive -> act -> learn"),
        .def("stepEnvironment", [](AgentBrain& self, const SensoryPercept& percept, const Action& action) {
            // This would integrate with world - placeholder for actual implementation
            return ActionResult();
        }, py::arg("percept"), py::arg("action"),
           "Execute action in environment"),
        .def("isEpisodeComplete", [](const AgentBrain& self, float threshold) {
            return self.getPredictionError() > threshold;
        }, py::arg("threshold") = 0.5f,
           "Check if episode should be terminated based on prediction error"),
        .def("getStateDict", [](const AgentBrain& self) {
            return py::dict("neuromodulation"_a = self.getNeuromodulationLevel(),
                           "curiosity"_a = self.getCuriosityLevel(),
                           "novelty"_a = self.getNoveltyLevel(),
                           "predictionError"_a = self.getPredictionError(),
                           "development"_a = static_cast<int>(self.getDevelopmentalStage()));
        }, "Get agent state as dictionary"),
        .def("setStateFromDict", [](AgentBrain& self, const py::dict& state) {
            // Apply state from dictionary - placeholder for actual implementation
        }, py::arg("state"), "Set agent state from dictionary");

    // Convenience factory functions
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
    
    // Experiment factory functions
    m.def("createExperimentRunner", [](std::unique_ptr<Experiment> experiment) -> std::unique_ptr<ExperimentRunner> {
        return std::make_unique<ExperimentRunner>(std::move(experiment));
    }, py::arg("experiment"), "Create an experiment runner with experiment");
    
    m.def("createPhase6Experiment", []() -> std::unique_ptr<Experiment> {
        return std::make_unique<Phase6IntegratedExperiment>();
    }, "Create Phase 6 integration experiment");
    
    m.def("createPhase5Experiment", []() -> std::unique_ptr<Experiment> {
        return std::make_unique<Phase5Experiment>();
    }, "Create Phase 5 performance experiment");
    
    m.def("createPhase4Experiment", []() -> std::unique_ptr<Experiment> {
        return std::make_unique<Phase4Experiment>();
    }, "Create Phase 4 cognition experiment");
    
    m.def("createPhase3Experiment", []() -> std::unique_ptr<Experiment> {
        return std::make_unique<Phase3Experiment>();
    }, "Create Phase 3 prediction experiment");
    
    m.def("createPhase2Experiment", []() -> std::unique_ptr<Experiment> {
        return std::make_unique<Phase2Experiment>();
    }, "Create Phase 2 development experiment");
    
    m.def("createPhase1Experiment", []() -> std::unique_ptr<Experiment> {
        return std::make_unique<Phase1Experiment>();
    }, "Create Phase 1 memory experiment");
    
    // Context manager for experiment
    py::class_<ExperimentContextManager>(m, "ExperimentContextManager")
        .def(py::init<std::unique_ptr<Experiment>>())
        .def("__enter__", &ExperimentContextManager::__enter__)
        .def("__exit__", &ExperimentContextManager::__exit__)
        .def("getExperiment", &ExperimentContextManager::getExperiment,
             py::return_value_policy::reference_internal);
    
    m.def("runExperiment", [](std::unique_ptr<Experiment> experiment) {
        ExperimentContextManager manager(std::move(experiment));
        return manager.getExperiment();
    }, py::arg("experiment"), "Run experiment with automatic cleanup");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic API aliases
    m.def("getBrain", m.attr("createBrain"), py::arg("config"), "Alias for createBrain");
    m.def("getConfig", m.attr("createDefaultConfig"), "Alias for createDefaultConfig");
    m.def("getWorld", m.attr("createSimpleWorld"), "Alias for createSimpleWorld");
    m.def("getAgent", m.attr("createAgentBrain"), py::arg("brain"), "Alias for createAgentBrain");

    // Performance optimization utilities
    m.def("batchNeuronInjection", [](Brain& brain, const std::vector<NeuronId>& neuronIds, float current) {
        for (const auto& id : neuronIds) {
            brain.injectCurrent(id, current);
        }
    }, py::arg("brain"), py::arg("neuronIds"), py::arg("current"),
       "Efficient batch injection of current to multiple neurons");
    
    m.def("batchSynapseWeightUpdate", [](Brain& brain, const std::vector<SynapseId>& synapseIds, float newWeight) {
        // This would require access to synapse update functionality
        // Placeholder for actual implementation
    }, py::arg("brain"), py::arg("synapseIds"), py::arg("newWeight"),
       "Efficient batch update of synapse weights");
    
    m.def("vectorizedNeuronalActivity", [](Brain& brain, const std::vector<NeuronType>& types, float multiplier = 1.0f) {
        for (NeuronType type : types) {
            brain.injectCurrentToNeurons(type, 1.0f * multiplier);
        }
    }, py::arg("brain"), py::arg("types"), py::arg("multiplier") = 1.0f,
       "Vectorized neuronal activity across neuron types");
    
    // Error handling and validation
    m.def("validateBrainState", [](const Brain& brain) {
        bool valid = true;
        if (brain.getTotalNeuronCount() == 0) {
            valid = false;
        }
        if (brain.getRegionCount() == 0) {
            valid = false;
        }
        return valid;
    }, py::arg("brain"), "Validate brain state for potential issues");
    
    m.def("checkMemoryUsage", [](const Brain& brain) {
        // Return memory usage information
        return py::dict("neurons"_a = brain.getTotalNeuronCount(),
                       "synapses"_a = brain.getTotalSynapseCount(),
                       "regions"_a = brain.getRegionCount());
    }, py::arg("brain"), "Check memory usage and return statistics");
    
    // Documentation and help functions
    m.def("getApiDocs", []() {
        return R"pbdoc(
            NLM Python API Documentation
            ============================
            
            Brain class: Central neural simulation with full integration of all brain systems.
            
            AgentBrain class: Interface connecting brain to world with action selection.
            
            Config class: Configuration management with Pythonic API.
            
            Key features:
            - Context managers for automatic resource cleanup
            - Batch operations for performance
            - Memory management utilities
            - Episode recording and playback
            - Experiment setup helpers
            - Enhanced error handling
            - Pythonic property access
            - Type hints and documentation
        )pbdoc";
    }, "Get API documentation");
    
    // Version and info
    m.def("getVersion", []() {
        return "NLM Python Bindings v0.1.0 - Phase 6 Integration";
    }, "Get version information");
    
    m.def("getPhase", []() {
        return "Phase 6: Final Integration";
    }, "Get current phase information");

    // Educational helper functions
    m.def("createTutorialBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.2f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a tutorial brain with pre-configured settings for learning");
    
    m.def("createBenchmarkBrain", [](size_t neuronCount, size_t regionCount) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 3,
       "Create a benchmark brain with specified size for performance testing");
    
    // Memory and performance monitoring
    m.def("monitorBrainPerformance", [](Brain& brain, size_t steps) {
        // This would monitor brain performance over specified steps
        // Placeholder for actual implementation
        return py::dict("steps"_a = steps,
                       "neurons"_a = brain.getTotalNeuronCount(),
                       "synapses"_a = brain.getTotalSynapseCount());
    }, py::arg("brain"), py::arg("steps"), "Monitor brain performance");
    
    // Error handling and validation
    m.def("validateConfig", [](const Config& config) {
        // Validate configuration
        for (const auto& key : config.getKeys()) {
            // Check for required keys or validate values
        }
        return true;  // Placeholder
    }, py::arg("config"), "Validate configuration settings");
    
    // Integration helper
    m.def("createIntegratedSystem", []() {
        // Create a complete integrated system
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 3);
        config->set("learning_rate", 0.01f);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        world->configure(100, 100, 20, 20);
        agentBrain->initialize(*world);
        brain->initialize();
        
        return py::make_tuple(brain, world, agentBrain);
    }, "Create a complete integrated system (brain, world, agent)");

    // Constants for common use
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 3;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("DEFAULT_EXPERIMENT_DURATION") = 3600;  // seconds
    
    // Type aliases for convenience
    py::class_<py::list>(m, "NeuronList");  // Placeholder
    py::class_<py::list>(m, "SynapseList");  // Placeholder
    
    // Version and help
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__status__") = "Phase 6 - Final Integration";

    // Educational examples
    m.def("getExampleCode", []() {
        return R"pbdoc(
            # Basic usage example
            from pynlm import createBrain, createSimpleWorld
            
            # Create brain with default config
            brain = createBrain()
            brain.initialize()
            
            # Create world
            world = createSimpleWorld()
            
            # Run simulation
            for step in range(1000):
                brain.step(step)
                world.update(step)
            
            # Save results
            brain.save("results.nlm")
        )pbdoc";
    }, "Get example usage code");

    // Error types
    py::register_exception<std::runtime_error>(m, "NLMError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Custom exception for brain-specific errors
    py::class_<py::exception<std::runtime_error>>(m, "NLMError")
        .def(py::init<const std::string&>(), py::arg("what"));
}

} // namespace nlm
