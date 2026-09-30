#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Memory system headers
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

// Prediction system headers
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/PredictionSystem.hpp"

// Neuromodulation headers
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"

// Plasticity headers
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

// Dynamics headers
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        Provides extended classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Includes enhanced memory systems, prediction integration, and performance optimizations.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Define fundamental types
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

    // Configuration class
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
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("setValue", [](Config& self, const std::string& key, const std::string& value) {
            self.setValue(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value")
        .def("getValue", [](Config& self, const std::string& key, const std::string& defaultValue = "") {
            return self.getValue(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.setValue(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration value")
        .def("getInt", [](Config& self, const std::string& key, int defaultValue = 0) {
            return std::stoi(self.getValue(key, std::to_string(defaultValue)));
        }, py::arg("key"), py::arg("defaultValue"),
           "Get integer configuration value")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.setValue(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"),
           "Set float configuration value")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue = 0.0f) {
            return std::stof(self.getValue(key, std::to_string(defaultValue)));
        }, py::arg("key"), py::arg("defaultValue"),
           "Get float configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.setValue(key, value ? "true" : "false");
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration value")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue = false) {
            std::string value = self.getValue(key, defaultValue ? "true" : "false");
            return value == "true" || value == "1" || value == "yes";
        }, py::arg("key"), py::arg("defaultValue"),
           "Get boolean configuration value");

    // Sensory input classes
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
        .def("getChannels", &Vision::getChannels)
        .def("getNormalizedData", &Vision::getNormalizedData)
        .def("computeFeatureMap", &Vision::computeFeatureMap);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getNormalizedData", &Audio::getNormalizedData)
        .def("computeMelSpectrogram", &Audio::computeMelSpectrogram);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"))
        .def("size", &InternalSignals::size)
        .def("getMean", &InternalSignals::getMean)
        .def("getStdDev", &InternalSignals::getStdDev)
        .def("getMax", &InternalSignals::getMax)
        .def("getMin", &InternalSignals::getMin);

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
        .def("getDuration", &Action::getDuration)
        .def("getPriority", &Action::getPriority)
        .def("setPriority", &Action::setPriority, py::arg("priority"));

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
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isCollidingWith", [](const WorldObject& obj1, const WorldObject& obj2) {
            float dx = obj1.x - obj2.x;
            float dy = obj1.y - obj2.y;
            return (dx * dx + dy * dy) <= (obj1.radius + obj2.radius) * (obj1.radius + obj2.radius);
        }, py::arg("other"), "Check collision with another object");

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
        .def("getPosition", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        })
        .def("getVelocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        })
        .def("updatePosition", [](AgentBody& body, float dt) {
            body.x += body.velocityX * dt;
            body.y += body.velocityY * dt;
            body.orientation += body.angularVelocity * dt;
        }, py::arg("dt"), "Update position based on velocity and angular velocity")
        .def("applyForce", [](AgentBody& body, float forceX, float forceY, float dt) {
            body.velocityX += forceX * dt;
            body.velocityY += forceY * dt;
            body.energy = std::max(0.0f, body.energy - 0.1f * dt);
        }, py::arg("forceX"), py::arg("forceY"), py::arg("dt"), "Apply force to agent body");

    // Action result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", &ActionResult::isPositive)
        .def("isNegative", &ActionResult::isNegative)
        .def("getConfidence", &ActionResult::getConfidence);

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
        .def("addNoise", [](SensoryPercept& percept, float amount) {
            percept.addNoise(amount);
        }, py::arg("amount"), "Add noise to sensory signals")
        .def("normalize", &SensoryPercept::normalize)
        .def("getSignalRange", &SensoryPercept::getSignalRange);

    // Simple world class
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
        .def("getNearestObject", [](const SimpleWorld& world, float x, float y) {
            return world.getNearestObject(x, y);
        }, py::arg("x"), py::arg("y"), "Get nearest object to position")
        .def("getObjectsInRadius", [](const SimpleWorld& world, float x, float y, float radius) {
            return world.getObjectsInRadius(x, y, radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), "Get objects in radius");

    // Enhanced Brain class with memory and prediction integration
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced neural simulation brain class with memory and prediction)pbdoc")
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
        // Memory system integration
        .def("getWorkingMemory", [](Brain& brain) {
            return brain.getWorkingMemory();
        }, "Get working memory system")
        .def("getEpisodicMemory", [](Brain& brain) {
            return brain.getEpisodicMemory();
        }, "Get episodic memory system")
        .def("getSemanticMemory", [](Brain& brain) {
            return brain.getSemanticMemory();
        }, "Get semantic memory system")
        .def("getProceduralMemory", [](Brain& brain) {
            return brain.getProceduralMemory();
        }, "Get procedural memory system")
        .def("getPredictionSystem", [](Brain& brain) {
            return brain.getPredictionSystem();
        }, "Get prediction system")
        .def("getNeuromodulatorSystem", [](Brain& brain) {
            return brain.getNeuromodulatorSystem();
        }, "Get neuromodulator system")
        .def("getPlasticitySystem", [](Brain& brain) {
            return brain.getPlasticitySystem();
        }, "Get plasticity system")
        .def("getSpikeSystem", [](Brain& brain) {
            return brain.getSpikeSystem();
        }, "Get spike system")
        // Memory convenience methods
        .def("storeEpisode", [](Brain& brain, const std::string& content, float importance = 1.0f) {
            return brain.storeEpisode(content, importance);
        }, py::arg("content"), py::arg("importance") = 1.0f, "Store an episode in episodic memory")
        .def("retrieveSimilarEpisodes", [](Brain& brain, const std::string& query, int maxResults = 10) {
            return brain.retrieveSimilarEpisodes(query, maxResults);
        }, py::arg("query"), py::arg("maxResults") = 10, "Retrieve similar episodes")
        .def("learnConcept", [](Brain& brain, const std::string& concept, const std::vector<float>& features) {
            return brain.learnConcept(concept, features);
        }, py::arg("concept"), py::arg("features"), "Learn a new semantic concept")
        .def("findConcept", [](Brain& brain, const std::vector<float>& features, float threshold = 0.5f) {
            return brain.findConcept(features, threshold);
        }, py::arg("features"), py::arg("threshold") = 0.5f, "Find similar concept")
        .def("learnSkill", [](Brain& brain, const std::string& skillName, const std::vector<float>& parameters) {
            return brain.learnSkill(skillName, parameters);
        }, py::arg("skillName"), py::arg("parameters"), "Learn a new procedural skill")
        .def("retrieveSkill", [](Brain& brain, const std::string& skillName) {
            return brain.retrieveSkill(skillName);
        }, py::arg("skillName"), "Retrieve a learned skill")
        .def("getAllSkills", [](Brain& brain) {
            return brain.getAllSkills();
        }, "Get all learned skills")
        // Prediction system integration
        .def("predictNextState", [](Brain& brain, const std::vector<float>& currentState) {
            return brain.predictNextState(currentState);
        }, py::arg("currentState"), "Predict next state")
        .def("getPredictionError", [](Brain& brain, const std::vector<float>& predicted, const std::vector<float>& actual) {
            return brain.getPredictionError(predicted, actual);
        }, py::arg("predicted"), py::arg("actual"), "Get prediction error")
        .def("getPredictionConfidence", [](Brain& brain, const std::vector<float>& state) {
            return brain.getPredictionConfidence(state);
        }, py::arg("state"), "Get prediction confidence")
        .def("updatePrediction", [](Brain& brain, const std::vector<float>& state, float reward) {
            return brain.updatePrediction(state, reward);
        }, py::arg("state"), py::arg("reward"), "Update prediction system")
        // Performance optimization methods
        .def("batchSensoryInput", [](Brain& brain, const std::vector<std::shared_ptr<SensoryInput>>& inputs) {
            brain.batchSensoryInput(inputs);
        }, py::arg("inputs"), "Process batch sensory input")
        .def("optimizeSpikeProcessing", [](Brain& brain, bool enable) {
            brain.optimizeSpikeProcessing(enable);
        }, py::arg("enable"), "Enable spike processing optimization")
        .def("getMemoryStats", [](Brain& brain) {
            return brain.getMemoryStats();
        }, "Get memory system statistics")
        .def("getPredictionStats", [](Brain& brain) {
            return brain.getPredictionStats();
        }, "Get prediction system statistics")
        .def("getPerformanceMetrics", [](Brain& brain) {
            return brain.getPerformanceMetrics();
        }, "Get performance metrics")
        // Advanced user commands
        .def("discoverConcepts", [](Brain& brain, int minSamples = 10) {
            return brain.discoverConcepts(minSamples);
        }, py::arg("minSamples") = 10, "Discover new concepts from data")
        .def("getPatternMatches", [](Brain& brain, const std::vector<float>& pattern, int maxResults = 5) {
            return brain.getPatternMatches(pattern, maxResults);
        }, py::arg("pattern"), py::arg("maxResults") = 5, "Find pattern matches")
        .def("enableMemoryReplay", [](Brain& brain, bool enable) {
            brain.enableMemoryReplay(enable);
        }, py::arg("enable"), "Enable memory replay")
        .def("enableConceptDiscovery", [](Brain& brain, bool enable) {
            brain.enableConceptDiscovery(enable);
        }, py::arg("enable"), "Enable concept discovery")
        .def("setDevelopmentStage", [](Brain& brain, const std::string& stageName) {
            brain.setDevelopmentStage(stageName);
        }, py::arg("stageName"), "Set development stage")
        .def("getAvailableStages", [](Brain& brain) {
            return brain.getAvailableStages();
        }, "Get available development stages")
        // Neuromodulation controls
        .def("enableRewardModulation", [](Brain& brain, bool enable) {
            brain.enableRewardModulation(enable);
        }, py::arg("enable"), "Enable reward modulation")
        .def("enableStructuralPlasticity", [](Brain& brain, bool enable) {
            brain.enableStructuralPlasticity(enable);
        }, py::arg("enable"), "Enable structural plasticity")
        .def("enableDevelopment", [](Brain& brain, bool enable) {
            brain.enableDevelopment(enable);
        }, py::arg("enable"), "Enable development")
        .def("enableCuriosity", [](Brain& brain, bool enable) {
            brain.enableCuriosity(enable);
        }, py::arg("enable"), "Enable curiosity")
        .def("isRewardModulationEnabled", &Brain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &Brain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &Brain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &Brain::isCuriosityEnabled)
        // Neuromodulator access
        .def("getDopamineLevel", &Brain::getDopamineLevel)
        .def("getCuriosityLevel", &Brain::getCuriosityLevel)
        .def("getNoveltyLevel", &Brain::getNoveltyLevel)
        .def("getPredictionErrorLevel", &Brain::getPredictionErrorLevel)
        .def("getRewardLevel", &Brain::getRewardLevel)
        .def("getAllNeuromodulationLevels", &Brain::getAllNeuromodulationLevels)
        // Plasticity system access
        .def("getSTDP", [](Brain& brain) {
            return brain.getSTDP();
        }, "Get STDP plasticity rule")
        .def("getHebbianRule", [](Brain& brain) {
            return brain.getHebbianRule();
        }, "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", [](Brain& brain) {
            return brain.getStructuralPlasticity();
        }, "Get structural plasticity system")
        .def("getPlasticityStats", [](Brain& brain) {
            return brain.getPlasticityStats();
        }, "Get plasticity system statistics")
        .def("resetPlasticity", [](Brain& brain) {
            brain.resetPlasticity();
        }, "Reset plasticity parameters");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with advanced memory and prediction)pbdoc")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Enhanced AgentBrain methods
        .def("processSensoryInputBatch", [](AgentBrain& agent, const std::vector<std::shared_ptr<SensoryInput>>& inputs) {
            agent.processSensoryInputBatch(inputs);
        }, py::arg("inputs"), "Process batch sensory input")
        .def("getActionWithMemory", [](AgentBrain& agent, const SensoryPercept& percept) {
            return agent.getActionWithMemory(pertcept);
        }, py::arg("percept"), "Get action with memory integration")
        .def("storePerceptInEpisodicMemory", [](AgentBrain& agent, const SensoryPercept& percept, float importance = 1.0f) {
            agent.storePerceptInEpisodicMemory(pertcept, importance);
        }, py::arg("percept"), py::arg("importance"), "Store percept in episodic memory")
        .def("retrieveFromEpisodicMemory", [](AgentBrain& agent, const std::string& query, int maxResults = 5) {
            return agent.retrieveFromEpisodicMemory(query, maxResults);
        }, py::arg("query"), py::arg("maxResults"), "Retrieve from episodic memory")
        .def("getWorkingMemorySummary", [](AgentBrain& agent) {
            return agent.getWorkingMemorySummary();
        }, "Get working memory summary")
        .def("getEpisodicMemoryStats", [](AgentBrain& agent) {
            return agent.getEpisodicMemoryStats();
        }, "Get episodic memory statistics")
        .def("getPredictionErrorWithContext", [](AgentBrain& agent, const std::vector<float>& state) {
            return agent.getPredictionErrorWithContext(state);
        }, py::arg("state"), "Get prediction error with context")
        .def("updateWithPrediction", [](AgentBrain& agent, const std::vector<float>& state, float reward) {
            return agent.updateWithPrediction(state, reward);
        }, py::arg("state"), py::arg("reward"), "Update with prediction")
        .def("getConceptDiscoveryResults", [](AgentBrain& agent) {
            return agent.getConceptDiscoveryResults();
        }, "Get concept discovery results")
        .def("getPatternRecognitionResults", [](AgentBrain& agent, const std::vector<float>& pattern) {
            return agent.getPatternRecognitionResults(pattern);
        }, py::arg("pattern"), "Get pattern recognition results")
        .def("getMemoryReplayStatus", [](AgentBrain& agent) {
            return agent.getMemoryReplayStatus();
        }, "Get memory replay status")
        .def("enableAdvancedLearning", [](AgentBrain& agent, bool enable) {
            agent.enableAdvancedLearning(enable);
        }, py::arg("enable"), "Enable advanced learning features")
        .def("getAdvancedLearningStats", [](AgentBrain& agent) {
            return agent.getAdvancedLearningStats();
        }, "Get advanced learning statistics")
        .def("getSkillLearningProgress", [](AgentBrain& agent) {
            return agent.getSkillLearningProgress();
        }, "Get skill learning progress")
        .def("getConceptLearningProgress", [](AgentBrain& agent) {
            return agent.getConceptLearningProgress();
        }, "Get concept learning progress")
        .def("getPatternRecognitionStats", [](AgentBrain& agent) {
            return agent.getPatternRecognitionStats();
        }, "Get pattern recognition statistics");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new enhanced brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new enhanced agent brain interface");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Statistical functions
    m.def("computeCorrelation", [](const std::vector<float>& x, const std::vector<float>& y) {
        return computeCorrelation(x, y);
    }, py::arg("x"), py::arg("y"), "Compute correlation between two vectors");

    m.def("computeMSE", [](const std::vector<float>& predicted, const std::vector<float>& actual) {
        return computeMSE(predicted, actual);
    }, py::arg("predicted"), py::arg("actual"), "Compute mean squared error");

    m.def("normalizeVector", [](const std::vector<float>& vec) {
        return normalizeVector(vec);
    }, py::arg("vec"), "Normalize vector");

    m.def("dotProduct", [](const std::vector<float>& a, const std::vector<float>& b) {
        return dotProduct(a, b);
    }, py::arg("a"), py::arg("b"), "Compute dot product");
}

} // namespace nlm
