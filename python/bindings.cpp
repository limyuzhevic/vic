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
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
             return "<Config: " + cfg.summary() + ">";
         });

    // Update Config class to include new JSON/YAML methods
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class with JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from JSON/YAML file (format auto-detected)")
        .def("loadFromString", &Config::loadFromString, py::arg("content"), py::arg("format") = "auto",
             "Load configuration from JSON/YAML string")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
             return self.loadFromArgs(argc, argv);
         }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file (format auto-detected)")
        .def("saveAsJSON", &Config::saveAsJSON, py::arg("filepath"),
             "Save configuration to JSON file")
        .def("saveAsYAML", &Config::saveAsYAML, py::arg("filepath"),
             "Save configuration to YAML file (placeholder)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("validate", &Config::validate,
             "Validate configuration")
        .def("getValidationErrors", &Config::getValidationErrors,
             "Get validation error messages")
        .def("getEntryCount", &Config::getEntryCount,
             "Get number of configuration entries")
        .def("getKeysBySource", &Config::getKeysBySource, py::arg("source"),
             "Get keys by configuration source")
        .def("getFormatInfo", &Config::getFormatInfo,
             "Get configuration format information")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
             self.set(key, value);
         }, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("set", [](Config& self, const std::string& key, int value) {
             self.set(key, value);
         }, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
             self.set(key, value);
         }, py::arg("key"), py::arg("value"),
             "Set double configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
             self.set(key, value);
         }, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("get", [](const Config& self, const std::string& key) -> std::optional<std::string> {
             auto val = self.get<std::string>(key);
             return val;
         }, py::arg("key"),
             "Get string configuration value")
        .def("get", [](const Config& self, const std::string& key) -> std::optional<int> {
             auto val = self.get<int>(key);
             return val;
         }, py::arg("key"),
             "Get integer configuration value")
        .def("get", [](const Config& self, const std::string& key) -> std::optional<double> {
             auto val = self.get<double>(key);
             return val;
         }, py::arg("key"),
             "Get double configuration value")
        .def("get", [](const Config& self, const std::string& key) -> std::optional<bool> {
             auto val = self.get<bool>(key);
             return val;
         }, py::arg("key"),
             "Get boolean configuration value")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
             return self.getOr<std::string>(key, defaultValue);
         }, py::arg("key"), py::arg("defaultValue"),
             "Get string value with default")
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
             return self.getOr<int>(key, defaultValue);
         }, py::arg("key"), py::arg("defaultValue"),
             "Get integer value with default")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
             return self.getOr<double>(key, defaultValue);
         }, py::arg("key"), py::arg("defaultValue"),
             "Get double value with default")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
             return self.getOr<bool>(key, defaultValue);
         }, py::arg("key"), py::arg("defaultValue"),
             "Get boolean value with default")
        .def("hasMetadata", &Config::hasMetadata, py::arg("key"),
             "Check if configuration has metadata")
        .def("setMetadata", &Config::setMetadata, py::arg("key"), py::arg("metadata"),
             "Set metadata for configuration key")
        .def("getMetadata", &Config::getMetadata, py::arg("key"),
             "Get metadata for configuration key")
        .def("copyFrom", [](Config& self, const Config& other) {
             self.clear();
             for (const auto& entry : other.getKeys()) {
                 auto val = other.get<std::string>(entry);
                 if (val) {
                     self.set(entry, *val);
                 }
             }
         }, py::arg("other"),
             "Copy configuration from another config object")
        .def("mergeFrom", [](Config& self, const Config& other) {
             for (const auto& entry : other.getKeys()) {
                 auto val = other.get<std::string>(entry);
                 if (val) {
                     self.set(entry, *val);
                 }
             }
         }, py::arg("other"),
             "Merge configuration from another config object")
        .def("diff", [](const Config& self, const Config& other) {
             nlohmann::json diff = nlohmann::json::object();
             for (const auto& key : self.getKeys()) {
                 auto selfVal = self.get<std::string>(key);
                 auto otherVal = other.get<std::string>(key);
                 if (selfVal != otherVal) {
                     diff[key] = *selfVal;
                 }
             }
             return diff.dump(2);
         }, py::arg("other"),
             "Get difference between this and another config")
        .def("clone", [](const Config& self) {
             Config result;
             result.copyFrom(self);
             return result;
         },
             "Create a copy of the configuration");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

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

    // Complete Neuron class with all essential methods
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron class implementing LIF dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"),
             "Create neuron with given ID")
        .def("getId", &Neuron::getId,
             "Get neuron ID")
        .def("getType", &Neuron::getType,
             "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"),
             "Set neuron type")
        .def("getState", &Neuron::getState,
             "Get neuron state (const reference)")
        .def("getState", &Neuron::getState, py::return_value_policy::reference_internal,
             "Get neuron state (non-const reference)")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"),
             "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"),
             "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold,
             "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"),
             "Set firing threshold")
        .def("isFiring", &Neuron::isFiring,
             "Check if neuron is currently firing")
        .def("isRefractory", &Neuron::isRefractory,
             "Check if neuron is in refractory period")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"),
             "Set neuron firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"),
             "Set refractory period in steps")
        .def("decrementRefractory", &Neuron::decrementRefractory,
             "Decrement refractory counter")
        .def("getFiringRate", &Neuron::getFiringRate,
             "Get firing rate")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"),
             "Set firing rate")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"),
             "Set leak conductance")
        .def("getLeakConductance", &Neuron::getLeakConductance,
             "Get leak conductance")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod,
             "Get refractory period")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"),
             "Set resting potential")
        .def("getRestingPotential", &Neuron::getRestingPotential,
             "Get resting potential")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"),
             "Set reset potential")
        .def("getResetPotential", &Neuron::getResetPotential,
             "Get reset potential")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if membrane potential exceeds threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime,
             "Get timestamp of last spike")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Perform LIF integration step, returns true if neuron fired")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"),
             "Receive excitatory input (postsynaptic potential)")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"),
             "Receive inhibitory input (postsynaptic potential)")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"),
             "Receive modulatory input")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"),
             "Inject external current into neuron")
        .def("getTotalCurrent", &Neuron::getTotalCurrent,
             "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent,
             "Clear total injected current")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             "Record a spike at given timestamp")
        .def("getSpikeHistory", &Neuron::getSpikeHistory,
             "Get spike history vector")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory,
             "Clear spike history")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime,
             "Get last spike time")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"),
             "Add incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"),
             "Add outgoing synapse")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses,
             "Get incoming synapses")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses,
             "Get outgoing synapses")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags,
             "Get plasticity flags")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, py::return_value_policy::reference_internal,
             "Get plasticity flags (non-const reference)")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable plasticity mechanisms")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"),
             "Set region ID")
        .def("getRegionId", &Neuron::getRegionId,
             "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"),
             "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId,
             "Get population ID")
        .def("step", &Neuron::step, py::arg("currentTime"),
             "Update neuron for one simulation step")
        .def("reset", &Neuron::reset,
             "Reset neuron to initial state")
        .def("initializeRandom", [](Neuron& self, std::shared_ptr<RandomGenerator> rng) {
             self.initializeRandom(*rng);
         }, py::arg("rng"),
             "Initialize neuron with random parameters")
        .def("__repr__", [](const Neuron& n) {
             return "<Neuron id=" + std::to_string(n.getId().value) + " type=" + std::to_string(static_cast<int>(n.getType())) + ">";
         });

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
