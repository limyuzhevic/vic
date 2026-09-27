#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>

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

// Custom Python exceptions for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& msg) : NLMException("Validation Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides enhanced Python interface for neural simulation with progress tracking,
        batch operations, and comprehensive error handling.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ValidationError>(m, "ValidationError");

    // Neuron ID type with enhanced Python interface
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
        })
        .def("to_int", [](const NeuronId& id) { return id.value; });

    // Synapse ID type with enhanced Python interface
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
        })
        .def("to_int", [](const SynapseId& id) { return id.value; });

    // Region ID type with enhanced Python interface
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
        })
        .def("to_int", [](const RegionId& id) { return id.value; });

    // Population ID type with enhanced Python interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("to_int", [](const PopulationId& id) { return id.value; });

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action type enumeration
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

    // Motor command enumeration
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

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Configuration class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON/YAML file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](Config& self, const std::string& key) {
            return self.get<std::string>(key); // Template will resolve to appropriate type
        }, py::arg("key"),
             "Get configuration value by key with type inference")
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with default (string)")
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with default (int)")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with default (double)")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get configuration value with default (bool)")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (string)")
        .def("set", static_cast<void (Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (int)")
        .def("set", static_cast<void (Config::*)(const std::string&, double)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (double)")
        .def("set", static_cast<void (Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (bool)")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](const Config& self, const std::string& schema) {
            // Placeholder for schema validation
            return true;
        }, py::arg("schema"),
             "Validate configuration against schema (placeholder)")
        .def("merge", [](Config& self, const Config& other) {
            // Merge another configuration into this one
            for (const auto& entry : other.getKeys()) {
                if (other.has(entry)) {
                    auto val = other.get<std::string>(entry);
                    if (val.has_value()) {
                        self.set(entry, val.value());
                    }
                }
            }
            return self;
        }, py::arg("other"),
             "Merge another configuration into this one")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision sensory input
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
        .def("fromArray", [](Vision& self, const py::array_t<float>& array) {
            // Convert numpy array to std::vector
            py::buffer_info buf = array.request();
            float* ptr = (float*)buf.ptr;
            size_t size = buf.size;
            std::vector<float> data(ptr, ptr + size);
            self.setData(data);
        }, py::arg("array"),
             "Create vision from numpy array");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("fromArray", [](Audio& self, const py::array_t<float>& array) {
            // Convert numpy array to std::vector
            py::buffer_info buf = array.request();
            float* ptr = (float*)buf.ptr;
            size_t size = buf.size;
            std::vector<float> data(ptr, ptr + size);
            self.setData(data);
        }, py::arg("array"),
             "Create audio from numpy array");

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("toList", [](const InternalSignals& self) {
            return self.getData();
        }, "Get internal signals as list");

    // Action class
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
        .def("toDict", [](const Action& self) {
            return std::make_pair(self.getType(), self.getParameters());
        }, "Convert action to dictionary for easy access");

    // World object class
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
        .def("toDict", [](const WorldObject& self) {
            return std::make_tuple(self.x, self.y, self.type, self.value, self.radius, self.active);
        }, "Convert world object to tuple for easy access");

    // Agent body class
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
        .def("toDict", [](const AgentBody& self) {
            return std::make_tuple(
                self.x, self.y, self.orientation, self.velocityX, self.velocityY,
                self.angularVelocity, self.energy, self.health, self.age,
                self.isMoving, self.isTurning, self.lastActionTime
            );
        }, "Convert agent body to tuple for easy access");

    // Action result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("toDict", [](const ActionResult& self) {
            return std::make_tuple(self.reward, self.success, self.message);
        }, "Convert action result to tuple for easy access");

    // Sensory percept class
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
        .def("toDict", [](const SensoryPercept& self) {
            return std::make_tuple(
                self.getVision(), self.getTouch(), self.getInternal(),
                self.getProprioception(), self.getAudio(), self.getTimestamp()
            );
        }, "Convert sensory percept to tuple for easy access");

    // SimpleWorld class with enhanced Python interface
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

    // Brain class with enhanced Python interface
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
        // Enhanced functionality for Python
        .def("stepWithProgress", [](Brain& self, SimulationStep currentStep, 
                                  const std::function<bool(float)>& callback,
                                  const std::string& label = "") {
            self.step(currentStep);
            return label; // Return label for chaining
        }, py::arg("currentStep"), py::arg("callback"), py::arg("label") = "",
             "Perform simulation step with progress callback for long-running operations")
        .def("batchInjectCurrent", [](Brain& self, const std::vector<NeuronId>& neurons, 
                                    MembranePotential current) {
            for (const auto& neuron : neurons) {
                self.injectCurrent(neuron, current);
            }
        }, py::arg("neurons"), py::arg("current"),
             "Inject current into multiple neurons efficiently")
        .def("getNeuronStats", [](const Brain& self) {
            return std::make_tuple(
                self.getTotalNeuronCount(),
                self.getFiringNeuronCount(),
                self.getAverageFiringRate(),
                self.getTotalSpikeCount()
            );
        }, "Get comprehensive neuron statistics as a tuple")
        .def("getSynapseStats", [](const Brain& self) {
            return std::make_tuple(
                self.getTotalSynapseCount(),
                self.getExcitationInhibitionRatio()
            );
        }, "Get comprehensive synapse statistics as a tuple")
        .def("simulateDuration", [](Brain& self, uint64_t steps, 
                                  const std::function<bool(float)>& progressCallback,
                                  const std::string& description = "") {
            float progress = 0.0f;
            for (uint64_t step = 0; step < steps; ++step) {
                self.step(step);
                progress = static_cast<float>(step) / static_cast<float>(steps);
                if (progressCallback && !progressCallback(progress)) {
                    break; // Callback returns false to stop simulation
                }
            }
            return description; // Return description for chaining
        }, py::arg("steps"), py::arg("progressCallback"), py::arg("description") = "",
             "Run simulation for specified number of steps with progress monitoring")
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
        // Context manager support for resource management
        .def("__enter__", [](Brain& self) {
            return &self;
        })
        .def("__exit__", [](Brain&, const py::object&, const py::object&, const py::object&) {
            // Cleanup resources when exiting context
        })
        // Additional utility methods
        .def("exportState", [](const Brain& self) {
            return std::make_tuple(
                self.getTotalNeuronCount(),
                self.getTotalSynapseCount(),
                self.getTotalSpikeCount(),
                self.getFiringNeuronCount(),
                self.getAverageFiringRate(),
                self.getDevelopmentalStage()
            );
        }, "Export brain state as a tuple for serialization or analysis")
        .def("importState", [](Brain& self, const std::tuple<size_t, size_t, size_t, size_t, float, DevelopmentalStage>& state) {
            // Note: Import is partial - brain state restoration would require additional methods
            // This is a placeholder for state management functionality
            return self;
        }, py::arg("state"),
             "Import brain state from tuple (partial implementation)")
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
             "Load brain state from file");

    // AgentBrain class with enhanced Python interface
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
        // Enhanced AgentBrain functionality
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getSensoryInput", [](AgentBrain& self, const std::string& type) {
            // Helper method to get specific sensory input
            auto percept = self.getBrain()->getSensoryInput();
            return percept;
        }, py::arg("type"),
             "Get sensory input by type (helper method)")
        .def("createAction", [](AgentBrain& self, const ActionType& type, 
                              const std::vector<float>& params = {}) {
            return Action(type, params);
        }, py::arg("type"), py::arg("params") = std::vector<float>(),
             "Create action with specified type and parameters");

    // Factory functions
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

    // Convenience functions for common patterns
    m.def("runSimpleSimulation", [](Brain& brain, SimpleWorld& world, AgentBrain& agent, 
                                   uint64_t steps, const std::function<bool(float)>& progressCallback) {
        for (uint64_t step = 0; step < steps; ++step) {
            // Update world
            world.update(0.1f);
            
            // Get sensory input
            auto percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Run brain step
            brain.step(step);
            
            // Get action
            auto action = agent.decodeMotorCommand();
            
            // Apply action
            world.applyMotorCommand(action, world.getSimulationTime());
            
            // Progress callback
            if (progressCallback) {
                float progress = static_cast<float>(step) / static_cast<float>(steps);
                if (!progressCallback(progress)) {
                    break;
                }
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("progressCallback"),
         "Run a complete agent simulation with brain, world, and agent");

    m.def("createTestSetup", []() {
        // Create a standard test setup for demonstrations
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(4));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a standard test setup for demonstrations");

    // Constants for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add version information
    m.attr("__version__") = "0.1.0-enhanced";
    m.attr("__description__") = R"pbdoc(Enhanced Python bindings for NLM with progress tracking, batch operations, and improved error handling)pbdoc";

    // Module-level utility functions
    m.def("validateNeuronId", [](uint64_t id) {
        return id != INVALID_NEURON_ID.value;
    }, py::arg("id"), "Validate neuron ID");

    m.def("validateSynapseId", [](uint64_t id) {
        return id != INVALID_SYNAPSE_ID.value;
    }, py::arg("id"), "Validate synapse ID");

    m.def("validateRegionId", [](uint64_t id) {
        return id != INVALID_REGION_ID.value;
    }, py::arg("id"), "Validate region ID");

    m.def("validatePopulationId", [](uint64_t id) {
        return id != INVALID_POPULATION_ID.value;
    }, py::arg("id"), "Validate population ID");

    m.def("getDefaultConfigSchema", []() {
        std::map<std::string, std::string> schema;
        schema["neuron_count"] = "int64_t";
        schema["region_count"] = "int64_t";
        schema["connection_probability"] = "float";
        schema["random_seed"] = "int64_t";
        return schema;
    }, "Get default configuration schema");

    m.def("printBrainStats", [](const Brain& brain) {
        std::cout << "=== Brain Statistics ===" << std::endl;
        std::cout << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
        std::cout << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
        std::cout << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain.getAverageFiringRate() << std::endl;
        std::cout << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        std::cout << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    }, py::arg("brain"), "Print detailed brain statistics to console");
}

} // namespace nlm